/*
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <asm/atomic.h>
#include <asm/ioctls.h>
#include "audio_utils_aio.h"

void q6_audio_cb(uint32_t opcode, uint32_t token,
		uint32_t *payload, void *priv)
{
	struct q6audio_aio *audio = (struct q6audio_aio *)priv;

	pr_debug("%s:opcode = %x token = 0x%x\n", __func__, opcode, token);
	switch (opcode) {
	case ASM_DATA_EVENT_WRITE_DONE:
	case ASM_DATA_EVENT_READ_DONE:
	case ASM_DATA_CMDRSP_EOS:
	case ASM_DATA_CMD_MEDIA_FORMAT_UPDATE:
	case ASM_STREAM_CMD_SET_ENCDEC_PARAM:
	case ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY:
	case ASM_DATA_EVENT_ENC_SR_CM_NOTIFY:
		audio_aio_cb(opcode, token, payload, audio);
		break;
#if 1
	case APR_BASIC_RSP_RESULT:
		audio_aio_cb(opcode, token, payload, audio);
		break;
#endif
	default:
		pr_debug("%s:Unhandled event = 0x%8x\n", __func__, opcode);
		break;
	}
}

void audio_aio_cb(uint32_t opcode, uint32_t token,
		uint32_t *payload,  void *priv)
{
	union msm_audio_event_payload e_payload;
	struct q6audio_aio *audio = (struct q6audio_aio *)priv;

	switch (opcode) {
	case ASM_DATA_EVENT_WRITE_DONE:
		pr_debug("%s[%p]:ASM_DATA_EVENT_WRITE_DONE token = 0x%x\n",
			__func__, audio, token);
		audio_aio_async_write_ack(audio, token, payload);
		break;
	case ASM_DATA_EVENT_READ_DONE:
		pr_debug("%s[%p]:ASM_DATA_EVENT_READ_DONE token = 0x%x\n",
			__func__, audio, token);
		audio_aio_async_read_ack(audio, token, payload);
		break;
	case ASM_DATA_CMDRSP_EOS:
		/* EOS Handle */
		pr_debug("%s[%p]:ASM_DATA_CMDRSP_EOS\n", __func__, audio);
		if (audio->feedback) { /* Non-Tunnel mode */
			audio->eos_rsp = 1;
			/* propagate input EOS i/p buffer,
			after receiving DSP acknowledgement */
			if (audio->eos_flag &&
				(audio->eos_write_payload.aio_buf.buf_addr)) {
				audio_aio_post_event(audio,
						AUDIO_EVENT_WRITE_DONE,
						audio->eos_write_payload);
				memset(&audio->eos_write_payload , 0,
					sizeof(union msm_audio_event_payload));
				audio->eos_flag = 0;
			}
		} else { /* Tunnel mode */
			audio->eos_rsp = 1;
			wake_up(&audio->write_wait);
			wake_up(&audio->cmd_wait);
		}
		break;
	case ASM_DATA_CMD_MEDIA_FORMAT_UPDATE:
	case ASM_STREAM_CMD_SET_ENCDEC_PARAM:
		pr_debug("%s[%p]:payload0[%x] payloa1d[%x]opcode= 0x%x\n",
			__func__, audio, payload[0], payload[1], opcode);
		break;
	case ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY:
	case ASM_DATA_EVENT_ENC_SR_CM_NOTIFY:
		pr_debug("%s[%p]: ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY, "

				"payload[0]-sr = %d, payload[1]-chl = %d, "
				"payload[2] = %d, payload[3] = %d\n", __func__,
				audio, payload[0], payload[1], payload[2],
				payload[3]);
		pr_debug("%s[%p]: ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY, sr(prev) = %d, chl(prev) = %d,",
		__func__, audio, audio->pcm_cfg.sample_rate,
		audio->pcm_cfg.channel_count);
		audio->pcm_cfg.sample_rate = payload[0];
		audio->pcm_cfg.channel_count = payload[1] & 0xFFFF;
		e_payload.stream_info.chan_info = audio->pcm_cfg.channel_count;
		e_payload.stream_info.sample_rate = audio->pcm_cfg.sample_rate;
		audio_aio_post_event(audio, AUDIO_EVENT_STREAM_INFO, e_payload);
		break;
#if 1
	case APR_BASIC_RSP_RESULT:
		switch (payload[0]) {
		case ASM_STREAM_CMD_FLUSH:
			if (payload[1] == ADSP_EOK) {
				pr_debug("%s: FLUSH CMD success\n", __func__);
				audio_aio_ioport_reset(audio);
				audio->wflush = 0;
				audio->rflush = 0;
			} else {
				pr_err("%s: FLUSH CMD failed with status:%d\n",
					__func__, payload[1]);
				audio_aio_ioport_reset(audio);
				audio->wflush = 0;
				audio->rflush = 0;
			}
			break;
		default:
			pr_debug("%s: cmd%x cmd_status:%d\n",
				__func__, payload[0], payload[1]);
		}
#endif
	default:
		break;
	}
}

/* Read buffer from DSP / Handle Ack from DSP */
void audio_aio_async_read_ack(struct q6audio_aio *audio, uint32_t token,
			uint32_t *payload)
{
	unsigned long flags;
	union msm_audio_event_payload event_payload;
	struct audio_aio_buffer_node *filled_buf;

	/* No active flush in progress */
	if (audio->rflush)
		return;

	/* Statistics of read */
	atomic_add(payload[2], &audio->in_bytes);
	atomic_add(payload[7], &audio->in_samples);

	spin_lock_irqsave(&audio->dsp_lock, flags);
	BUG_ON(list_empty(&audio->in_queue));
	filled_buf = list_first_entry(&audio->in_queue,
					struct audio_aio_buffer_node, list);
	if (token == (filled_buf->token)) {
		list_del(&filled_buf->list);
		spin_unlock_irqrestore(&audio->dsp_lock, flags);
		event_payload.aio_buf = filled_buf->buf;
		/* Read done Buffer due to flush/normal condition
		after EOS event, so append EOS buffer */
		if (audio->eos_rsp == 0x1) {
			event_payload.aio_buf.data_len =
			insert_eos_buf(audio, filled_buf);
			/* Reset flag back to indicate eos intimated */
			audio->eos_rsp = 0;
		} else {
			filled_buf->meta_info.meta_out.num_of_frames =
			payload[7];
			event_payload.aio_buf.data_len = payload[2] + \
						payload[3] + \
						sizeof(struct dec_meta_out);
			pr_debug("%s[%p]:nr of frames 0x%8x len=%d\n",
				__func__, audio,
				filled_buf->meta_info.meta_out.num_of_frames,
				event_payload.aio_buf.data_len);
			extract_meta_out_info(audio, filled_buf, 0);
			audio->eos_rsp = 0;
		}
		audio_aio_post_event(audio, AUDIO_EVENT_READ_DONE,
					event_payload);
		kfree(filled_buf);
	} else {
		pr_err("%s[%p]:expected=%lx ret=%x\n",
			__func__, audio, filled_buf->token, token);
		spin_unlock_irqrestore(&audio->dsp_lock, flags);
	}
}
