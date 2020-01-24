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

#include "context.h"
#include "s5c-build.h"

#include "ipfw/ipfw2.h"

static int16_t smf_pco_build(uint8_t *pco_buf, ogs_gtp_tlv_pco_t *tlv_pco);

ogs_pkbuf_t *smf_s5c_build_create_session_response(
        uint8_t type, smf_sess_t *sess,
        ogs_diam_gx_message_t *gx_message)
{
    int rv;
    smf_bearer_t *bearer = NULL;

    ogs_gtp_message_t gtp_message;
    ogs_gtp_create_session_response_t *rsp = NULL;

    ogs_gtp_cause_t cause;
    ogs_gtp_f_teid_t smf_s5c_teid, smf_s5u_teid;
    int len;
    uint8_t pco_buf[OGS_MAX_PCO_LEN];
    int16_t pco_len;

    ogs_debug("[SMF] Create Session Response");

    ogs_assert(sess);
    bearer = smf_default_bearer_in_sess(sess);
    ogs_assert(bearer);

    ogs_debug("    SGW_S5C_TEID[0x%x] SMF_S5C_TEID[0x%x]",
            sess->sgw_s5c_teid, sess->smf_s5c_teid);
    ogs_debug("    SGW_S5U_TEID[%d] SMF_S5U_TEID[%d]",
            bearer->sgw_s5u_teid, bearer->smf_s5u_teid);

    rsp = &gtp_message.create_session_response;
    memset(&gtp_message, 0, sizeof(ogs_gtp_message_t));

    /* Set Cause */
    memset(&cause, 0, sizeof(cause));
    cause.value = OGS_GTP_CAUSE_REQUEST_ACCEPTED;
    rsp->cause.presence = 1;
    rsp->cause.len = sizeof(cause);
    rsp->cause.data = &cause;

    /* Control Plane(UL) : SMF-S5C */
    memset(&smf_s5c_teid, 0, sizeof(ogs_gtp_f_teid_t));
    smf_s5c_teid.interface_type = OGS_GTP_F_TEID_S5_S8_PGW_GTP_C;
    smf_s5c_teid.teid = htobe32(sess->smf_s5c_teid);
    rv = ogs_gtp_sockaddr_to_f_teid(
        smf_self()->gtpc_addr, smf_self()->gtpc_addr6, &smf_s5c_teid, &len);
    ogs_assert(rv == OGS_OK);
    rsp->pgw_s5_s8__s2a_s2b_f_teid_for_pmip_based_interface_or_for_gtp_based_control_plane_interface.
        presence = 1;
    rsp->pgw_s5_s8__s2a_s2b_f_teid_for_pmip_based_interface_or_for_gtp_based_control_plane_interface.
        data = &smf_s5c_teid;
    rsp->pgw_s5_s8__s2a_s2b_f_teid_for_pmip_based_interface_or_for_gtp_based_control_plane_interface.
        len = len;

    /* PDN Address Allocation */
    rsp->pdn_address_allocation.data = &sess->pdn.paa;
    if (sess->ipv4 && sess->ipv6)
        rsp->pdn_address_allocation.len = OGS_PAA_IPV4V6_LEN;
    else if (sess->ipv4)
        rsp->pdn_address_allocation.len = OGS_PAA_IPV4_LEN;
    else if (sess->ipv6)
        rsp->pdn_address_allocation.len = OGS_PAA_IPV6_LEN;
    else
        ogs_assert_if_reached();
    rsp->pdn_address_allocation.presence = 1;

    /* APN Restriction */
    rsp->apn_restriction.presence = 1;
    rsp->apn_restriction.u8 = OGS_GTP_APN_NO_RESTRICTION;
    
    /* TODO : APN-AMBR
     * if PCRF changes APN-AMBR, this should be included. */

    /* PCO */
    if (sess->ue_pco.presence && sess->ue_pco.len && sess->ue_pco.data) {
        pco_len = smf_pco_build(pco_buf, &sess->ue_pco);
        ogs_assert(pco_len > 0);
        rsp->protocol_configuration_options.presence = 1;
        rsp->protocol_configuration_options.data = pco_buf;
        rsp->protocol_configuration_options.len = pco_len;
    }

    /* Bearer EBI */
    rsp->bearer_contexts_created.presence = 1;
    rsp->bearer_contexts_created.eps_bearer_id.presence = 1;
    rsp->bearer_contexts_created.eps_bearer_id.u8 = bearer->ebi;

    /* Bearer Cause */
    rsp->bearer_contexts_created.cause.presence = 1;
    rsp->bearer_contexts_created.cause.len = sizeof(cause);
    rsp->bearer_contexts_created.cause.data = &cause;

    /* TODO : Bearer QoS 
     * if PCRF changes Bearer QoS, this should be included. */

    /* Data Plane(UL) : SMF-S5U */
#if 0
    memset(&smf_s5u_teid, 0, sizeof(ogs_gtp_f_teid_t));
    smf_s5u_teid.interface_type = OGS_GTP_F_TEID_S5_S8_PGW_GTP_U;
    smf_s5u_teid.teid = htobe32(bearer->smf_s5u_teid);
    rv = ogs_gtp_sockaddr_to_f_teid(
        smf_self()->gtpu_addr, smf_self()->gtpu_addr6, &smf_s5u_teid, &len);
    ogs_assert(rv == OGS_OK);
    rsp->bearer_contexts_created.s5_s8_u_sgw_f_teid.presence = 1;
    rsp->bearer_contexts_created.s5_s8_u_sgw_f_teid.data = &smf_s5u_teid;
    rsp->bearer_contexts_created.s5_s8_u_sgw_f_teid.len = len;
#endif

    gtp_message.h.type = type;
    return ogs_gtp_build_msg(&gtp_message);
}

ogs_pkbuf_t *smf_s5c_build_delete_session_response(
        uint8_t type, smf_sess_t *sess,
        ogs_diam_gx_message_t *gx_message)
{
    ogs_gtp_message_t gtp_message;
    ogs_gtp_delete_session_response_t *rsp = NULL;

    ogs_gtp_cause_t cause;
    uint8_t pco_buf[OGS_MAX_PCO_LEN];
    int16_t pco_len;
    
    ogs_assert(gx_message);

    /* prepare cause */
    memset(&cause, 0, sizeof(cause));
    cause.value = OGS_GTP_CAUSE_REQUEST_ACCEPTED;

    rsp = &gtp_message.delete_session_response;
    memset(&gtp_message, 0, sizeof(ogs_gtp_message_t));

    /* Cause */
    rsp->cause.presence = 1;
    rsp->cause.len = sizeof(cause);
    rsp->cause.data = &cause;

    /* Recovery */

    /* PCO */
    if (sess->ue_pco.presence && sess->ue_pco.len && sess->ue_pco.data) {
        pco_len = smf_pco_build(pco_buf, &sess->ue_pco);
        ogs_assert(pco_len > 0);
        rsp->protocol_configuration_options.presence = 1;
        rsp->protocol_configuration_options.data = pco_buf;
        rsp->protocol_configuration_options.len = pco_len;
    }

    /* Private Extension */

    /* build */
    gtp_message.h.type = type;
    return ogs_gtp_build_msg(&gtp_message);
}

ogs_pkbuf_t *smf_s5c_build_create_bearer_request(
        uint8_t type, smf_bearer_t *bearer, ogs_gtp_tft_t *tft)
{
    int rv;
    smf_sess_t *sess = NULL;
    smf_bearer_t *linked_bearer = NULL;

    ogs_gtp_message_t gtp_message;
    ogs_gtp_create_bearer_request_t *req = NULL;

    ogs_gtp_f_teid_t smf_s5u_teid;
    ogs_gtp_bearer_qos_t bearer_qos;
    char bearer_qos_buf[GTP_BEARER_QOS_LEN];
    int len;
    char tft_buf[OGS_GTP_MAX_TRAFFIC_FLOW_TEMPLATE];

    ogs_assert(bearer);
    sess = bearer->sess;
    ogs_assert(sess);
    linked_bearer = smf_default_bearer_in_sess(sess);
    ogs_assert(linked_bearer);

    ogs_debug("[SMF] Create Bearer Request");
    ogs_debug("    SGW_S5C_TEID[0x%x] SMF_S5C_TEID[0x%x]",
            sess->sgw_s5c_teid, sess->smf_s5c_teid);

    req = &gtp_message.create_bearer_request;
    memset(&gtp_message, 0, sizeof(ogs_gtp_message_t));
 
    /* Linked EBI */
    req->linked_eps_bearer_id.presence = 1;
    req->linked_eps_bearer_id.u8 = linked_bearer->ebi;

    /* Bearer EBI */
    req->bearer_contexts.presence = 1;
    req->bearer_contexts.eps_bearer_id.presence = 1;
    req->bearer_contexts.eps_bearer_id.u8 = bearer->ebi;

    /* Data Plane(UL) : SMF_S5U */
#if 0
    memset(&smf_s5u_teid, 0, sizeof(ogs_gtp_f_teid_t));
    smf_s5u_teid.interface_type = OGS_GTP_F_TEID_S5_S8_PGW_GTP_U;
    smf_s5u_teid.teid = htobe32(bearer->smf_s5u_teid);
    rv = ogs_gtp_sockaddr_to_f_teid(
        smf_self()->gtpu_addr, smf_self()->gtpu_addr6, &smf_s5u_teid, &len);
    ogs_assert(rv == OGS_OK);
    req->bearer_contexts.s5_s8_u_sgw_f_teid.presence = 1;
    req->bearer_contexts.s5_s8_u_sgw_f_teid.data = &smf_s5u_teid;
    req->bearer_contexts.s5_s8_u_sgw_f_teid.len = len;
#endif

    /* Bearer QoS */
    memset(&bearer_qos, 0, sizeof(bearer_qos));
    bearer_qos.qci = bearer->qos.qci;
    bearer_qos.priority_level = bearer->qos.arp.priority_level;
    bearer_qos.pre_emption_capability = 
        bearer->qos.arp.pre_emption_capability;
    bearer_qos.pre_emption_vulnerability =
        bearer->qos.arp.pre_emption_vulnerability;
    bearer_qos.dl_mbr = bearer->qos.mbr.downlink;
    bearer_qos.ul_mbr = bearer->qos.mbr.uplink;
    bearer_qos.dl_gbr = bearer->qos.gbr.downlink;
    bearer_qos.ul_gbr = bearer->qos.gbr.uplink;

    req->bearer_contexts.bearer_level_qos.presence = 1;
    ogs_gtp_build_bearer_qos(&req->bearer_contexts.bearer_level_qos,
            &bearer_qos, bearer_qos_buf, GTP_BEARER_QOS_LEN);

    /* Bearer TFT */
    if (tft && tft->num_of_packet_filter) {
        req->bearer_contexts.tft.presence = 1;
        ogs_gtp_build_tft(&req->bearer_contexts.tft,
                tft, tft_buf, OGS_GTP_MAX_TRAFFIC_FLOW_TEMPLATE);
    }

    gtp_message.h.type = type;
    return ogs_gtp_build_msg(&gtp_message);
}

ogs_pkbuf_t *smf_s5c_build_update_bearer_request(
        uint8_t type, smf_bearer_t *bearer, uint8_t pti,
        ogs_gtp_tft_t *tft, int qos_presence)
{
    smf_sess_t *sess = NULL;

    ogs_gtp_message_t gtp_message;
    ogs_gtp_update_bearer_request_t *req = NULL;

    ogs_gtp_ambr_t ambr;
    ogs_gtp_bearer_qos_t bearer_qos;
    char bearer_qos_buf[GTP_BEARER_QOS_LEN];
    char tft_buf[OGS_GTP_MAX_TRAFFIC_FLOW_TEMPLATE];

    ogs_assert(bearer);
    sess = bearer->sess;
    ogs_assert(sess);

    ogs_debug("[SMF] Update Bearer Request");
    ogs_debug("    SGW_S5C_TEID[0x%x] SMF_S5C_TEID[0x%x]",
            sess->sgw_s5c_teid, sess->smf_s5c_teid);
    req = &gtp_message.update_bearer_request;
    memset(&gtp_message, 0, sizeof(ogs_gtp_message_t));
 
    /* Bearer EBI */
    req->bearer_contexts.presence = 1;
    req->bearer_contexts.eps_bearer_id.presence = 1;
    req->bearer_contexts.eps_bearer_id.u8 = bearer->ebi;

    if (sess->pdn.ambr.uplink || sess->pdn.ambr.downlink) {
        /*
         * Ch 8.7. Aggregate Maximum Bit Rate(AMBR) in TS 29.274 V15.9.0
         *
         * AMBR is defined in clause 9.9.4.2 of 3GPP TS 24.301 [23],
         * but it shall be encoded as shown in Figure 8.7-1 as
         * Unsigned32 binary integer values in kbps (1000 bits per second).
         */
        memset(&ambr, 0, sizeof(ogs_gtp_ambr_t));
        ambr.uplink = htobe32(sess->pdn.ambr.uplink / 1000);
        ambr.downlink = htobe32(sess->pdn.ambr.downlink / 1000);
        req->aggregate_maximum_bit_rate.presence = 1;
        req->aggregate_maximum_bit_rate.data = &ambr;
        req->aggregate_maximum_bit_rate.len = sizeof(ambr);
    }

    /* PTI */
    if (pti) {
        req->procedure_transaction_id.presence = 1;
        req->procedure_transaction_id.u8 = pti;
    }

    /* Bearer QoS */
    if (qos_presence == 1) {
        memset(&bearer_qos, 0, sizeof(bearer_qos));
        bearer_qos.qci = bearer->qos.qci;
        bearer_qos.priority_level = bearer->qos.arp.priority_level;
        bearer_qos.pre_emption_capability = 
            bearer->qos.arp.pre_emption_capability;
        bearer_qos.pre_emption_vulnerability =
            bearer->qos.arp.pre_emption_vulnerability;
        bearer_qos.dl_mbr = bearer->qos.mbr.downlink;
        bearer_qos.ul_mbr = bearer->qos.mbr.uplink;
        bearer_qos.dl_gbr = bearer->qos.gbr.downlink;
        bearer_qos.ul_gbr = bearer->qos.gbr.uplink;

        req->bearer_contexts.bearer_level_qos.presence = 1;
        ogs_gtp_build_bearer_qos(&req->bearer_contexts.bearer_level_qos,
                &bearer_qos, bearer_qos_buf, GTP_BEARER_QOS_LEN);
    }

    /* Bearer TFT */
    if (tft && tft->num_of_packet_filter) {
        req->bearer_contexts.tft.presence = 1;
        ogs_gtp_build_tft(&req->bearer_contexts.tft,
                tft, tft_buf, OGS_GTP_MAX_TRAFFIC_FLOW_TEMPLATE);
    }

    gtp_message.h.type = type;
    return ogs_gtp_build_msg(&gtp_message);
}

ogs_pkbuf_t *smf_s5c_build_delete_bearer_request(
        uint8_t type, smf_bearer_t *bearer, uint8_t pti)
{
    smf_sess_t *sess = NULL;
    smf_bearer_t *linked_bearer = NULL;

    ogs_gtp_message_t gtp_message;
    ogs_gtp_delete_bearer_request_t *req = NULL;

    ogs_assert(bearer);
    sess = bearer->sess;
    ogs_assert(sess);
    linked_bearer = smf_default_bearer_in_sess(sess);
    ogs_assert(linked_bearer);

    ogs_debug("[SMF] Delete Bearer Request");
    ogs_debug("    SGW_S5C_TEID[0x%x] SMF_S5C_TEID[0x%x]",
            sess->sgw_s5c_teid, sess->smf_s5c_teid);
    req = &gtp_message.delete_bearer_request;
    memset(&gtp_message, 0, sizeof(ogs_gtp_message_t));
 
    if (bearer->ebi == linked_bearer->ebi) {
        /* Linked EBI */
        req->linked_eps_bearer_id.presence = 1;
        req->linked_eps_bearer_id.u8 = bearer->ebi;
    } else {
        /* Bearer EBI */
        req->eps_bearer_ids.presence = 1;
        req->eps_bearer_ids.u8 = bearer->ebi;
    }

    if (pti) {
        req->procedure_transaction_id.presence = 1;
        req->procedure_transaction_id.u8 = pti;
    }

    gtp_message.h.type = type;
    return ogs_gtp_build_msg(&gtp_message);
}

static int16_t smf_pco_build(uint8_t *pco_buf, ogs_gtp_tlv_pco_t *tlv_pco)
{
    int rv;
    ogs_pco_t ue, smf;
    ogs_pco_ipcp_t pco_ipcp;
    ogs_ipsubnet_t dns_primary, dns_secondary, dns6_primary, dns6_secondary;
    ogs_ipsubnet_t p_cscf, p_cscf6;
    int size = 0;
    int i = 0;

    ogs_assert(pco_buf);
    ogs_assert(tlv_pco);

    size = ogs_pco_parse(&ue, tlv_pco->data, tlv_pco->len);
    ogs_assert(size);

    memset(&smf, 0, sizeof(ogs_pco_t));
    smf.ext = ue.ext;
    smf.configuration_protocol = ue.configuration_protocol;

    for (i = 0; i < ue.num_of_id; i++) {
        uint8_t *data = ue.ids[i].data;
        switch(ue.ids[i].id) {
        case OGS_PCO_ID_CHALLENGE_HANDSHAKE_AUTHENTICATION_PROTOCOL:
            if (data[0] == 2) { /* Code : Response */
                smf.ids[smf.num_of_id].id = ue.ids[i].id;
                smf.ids[smf.num_of_id].len = 4;
                smf.ids[smf.num_of_id].data =
                    (uint8_t *)"\x03\x00\x00\x04"; /* Code : Success */
                smf.num_of_id++;
            }
            break;
        case OGS_PCO_ID_INTERNET_PROTOCOL_CONTROL_PROTOCOL:
            if (data[0] == 1) { /* Code : Configuration Request */
                uint16_t len = 16;

                memset(&pco_ipcp, 0, sizeof(ogs_pco_ipcp_t));
                pco_ipcp.code = 2; /* Code : Configuration Ack */
                pco_ipcp.len = htobe16(len);

                /* Primary DNS Server IP Address */
                if (smf_self()->dns[0]) {
                    rv = ogs_ipsubnet(
                            &dns_primary, smf_self()->dns[0], NULL);
                    ogs_assert(rv == OGS_OK);
                    pco_ipcp.options[0].type = 129; 
                    pco_ipcp.options[0].len = 6; 
                    pco_ipcp.options[0].addr = dns_primary.sub[0];
                }

                /* Secondary DNS Server IP Address */
                if (smf_self()->dns[1]) {
                    rv = ogs_ipsubnet(
                            &dns_secondary, smf_self()->dns[1], NULL);
                    ogs_assert(rv == OGS_OK);
                    pco_ipcp.options[1].type = 131; 
                    pco_ipcp.options[1].len = 6; 
                    pco_ipcp.options[1].addr = dns_secondary.sub[0];
                }

                smf.ids[smf.num_of_id].id = ue.ids[i].id;
                smf.ids[smf.num_of_id].len = len;
                smf.ids[smf.num_of_id].data = (uint8_t *)&pco_ipcp;

                smf.num_of_id++;
            }
            break;
        case OGS_PCO_ID_DNS_SERVER_IPV4_ADDRESS_REQUEST:
            if (smf_self()->dns[0]) {
                rv = ogs_ipsubnet(
                        &dns_primary, smf_self()->dns[0], NULL);
                ogs_assert(rv == OGS_OK);
                smf.ids[smf.num_of_id].id = ue.ids[i].id;
                smf.ids[smf.num_of_id].len = OGS_IPV4_LEN;
                smf.ids[smf.num_of_id].data = dns_primary.sub;
                smf.num_of_id++;
            }

            if (smf_self()->dns[1]) {
                rv = ogs_ipsubnet(
                        &dns_secondary, smf_self()->dns[1], NULL);
                ogs_assert(rv == OGS_OK);
                smf.ids[smf.num_of_id].id = ue.ids[i].id;
                smf.ids[smf.num_of_id].len = OGS_IPV4_LEN;
                smf.ids[smf.num_of_id].data = dns_secondary.sub;
                smf.num_of_id++;
            }
            break;
        case OGS_PCO_ID_DNS_SERVER_IPV6_ADDRESS_REQUEST:
            if (smf_self()->dns6[0]) {
                rv = ogs_ipsubnet(
                        &dns6_primary, smf_self()->dns6[0], NULL);
                ogs_assert(rv == OGS_OK);
                smf.ids[smf.num_of_id].id = ue.ids[i].id;
                smf.ids[smf.num_of_id].len = OGS_IPV6_LEN;
                smf.ids[smf.num_of_id].data = dns6_primary.sub;
                smf.num_of_id++;
            }

            if (smf_self()->dns6[1]) {
                rv = ogs_ipsubnet(
                        &dns6_secondary, smf_self()->dns6[1], NULL);
                ogs_assert(rv == OGS_OK);
                smf.ids[smf.num_of_id].id = ue.ids[i].id;
                smf.ids[smf.num_of_id].len = OGS_IPV6_LEN;
                smf.ids[smf.num_of_id].data = dns6_secondary.sub;
                smf.num_of_id++;
            }
            break;
        case OGS_PCO_ID_P_CSCF_IPV4_ADDRESS_REQUEST:
            if (smf_self()->num_of_p_cscf) {
                rv = ogs_ipsubnet(&p_cscf,
                    smf_self()->p_cscf[smf_self()->p_cscf_index], NULL);
                ogs_assert(rv == OGS_OK);
                smf.ids[smf.num_of_id].id = ue.ids[i].id;
                smf.ids[smf.num_of_id].len = OGS_IPV4_LEN;
                smf.ids[smf.num_of_id].data = p_cscf.sub;
                smf.num_of_id++;

                smf_self()->p_cscf_index++;
                smf_self()->p_cscf_index %= smf_self()->num_of_p_cscf;
            }
            break;
        case OGS_PCO_ID_P_CSCF_IPV6_ADDRESS_REQUEST:
            if (smf_self()->num_of_p_cscf6) {
                rv = ogs_ipsubnet(&p_cscf6,
                    smf_self()->p_cscf6[smf_self()->p_cscf6_index], NULL);
                ogs_assert(rv == OGS_OK);
                smf.ids[smf.num_of_id].id = ue.ids[i].id;
                smf.ids[smf.num_of_id].len = OGS_IPV6_LEN;
                smf.ids[smf.num_of_id].data = p_cscf6.sub;
                smf.num_of_id++;

                smf_self()->p_cscf6_index++;
                smf_self()->p_cscf6_index %= smf_self()->num_of_p_cscf6;
            }
            break;
        case OGS_PCO_ID_IP_ADDRESS_ALLOCATION_VIA_NAS_SIGNALLING:
            /* TODO */
            break;
        case OGS_PCO_ID_IPV4_LINK_MTU_REQUEST:
            /* TODO */
            break;
        default:
            ogs_warn("Unknown PCO ID:(0x%x)", ue.ids[i].id);
        }
    }

    size = ogs_pco_build(pco_buf, OGS_MAX_PCO_LEN, &smf);
    return size;
}