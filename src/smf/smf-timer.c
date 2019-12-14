/*
 * Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "smf-timer.h"
#include "smf-event.h"
#include "smf-context.h"

static smf_timer_cfg_t g_smf_timer_cfg[MAX_NUM_OF_SMF_TIMER] = {
    [SMF_TIMER_CONNECT_TO_UPF] = 
        { .duration = ogs_time_from_sec(12) },
};

smf_timer_cfg_t *smf_timer_cfg(smf_timer_e id)
{
    ogs_assert(id < MAX_NUM_OF_SMF_TIMER);
    return &g_smf_timer_cfg[id];
}

const char *smf_timer_get_name(smf_timer_e id)
{
    switch (id) {
    case SMF_TIMER_CONNECT_TO_UPF:
        return "SMF_TIMER_CONNECT_TO_UPF";
    default: 
       break;
    }

    return "UNKNOWN_TIMER";
}

void smf_timer_connect_to_upf(void *data)
{
    int rv;
    smf_event_t *e = NULL;
    ogs_assert(data);

    e = smf_event_new(SMF_EVT_N4_TIMER);
    e->timer_id = SMF_TIMER_CONNECT_TO_UPF;
    e->pnode = data;

    rv = ogs_queue_push(smf_self()->queue, e);
    if (rv != OGS_OK) {
        ogs_warn("ogs_queue_push() failed:%d", (int)rv);
        smf_event_free(e);
    }
}