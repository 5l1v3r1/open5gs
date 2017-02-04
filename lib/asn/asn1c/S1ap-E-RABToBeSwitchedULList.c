/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-PDU-Contents"
 * 	found in "../support/S1AP-PDU.asn"
 * 	`asn1c -fcompound-names -fincludes-quoted`
 */

#include "S1ap-E-RABToBeSwitchedULList.h"

int
S1ap_E_RABToBeSwitchedULList_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size >= 1LL && size <= 256LL)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, ctfailcb, app_key);
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

/*
 * This type is implemented using ProtocolIE_ContainerList_5937P0,
 * so here we adjust the DEF accordingly.
 */
static void
S1ap_E_RABToBeSwitchedULList_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_ProtocolIE_ContainerList_5937P0.free_struct;
	td->print_struct   = asn_DEF_ProtocolIE_ContainerList_5937P0.print_struct;
	td->check_constraints = asn_DEF_ProtocolIE_ContainerList_5937P0.check_constraints;
	td->ber_decoder    = asn_DEF_ProtocolIE_ContainerList_5937P0.ber_decoder;
	td->der_encoder    = asn_DEF_ProtocolIE_ContainerList_5937P0.der_encoder;
	td->xer_decoder    = asn_DEF_ProtocolIE_ContainerList_5937P0.xer_decoder;
	td->xer_encoder    = asn_DEF_ProtocolIE_ContainerList_5937P0.xer_encoder;
	td->uper_decoder   = asn_DEF_ProtocolIE_ContainerList_5937P0.uper_decoder;
	td->uper_encoder   = asn_DEF_ProtocolIE_ContainerList_5937P0.uper_encoder;
	td->aper_decoder   = asn_DEF_ProtocolIE_ContainerList_5937P0.aper_decoder;
	td->aper_encoder   = asn_DEF_ProtocolIE_ContainerList_5937P0.aper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_ProtocolIE_ContainerList_5937P0.per_constraints;
	td->elements       = asn_DEF_ProtocolIE_ContainerList_5937P0.elements;
	td->elements_count = asn_DEF_ProtocolIE_ContainerList_5937P0.elements_count;
	td->specifics      = asn_DEF_ProtocolIE_ContainerList_5937P0.specifics;
}

void
S1ap_E_RABToBeSwitchedULList_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	S1ap_E_RABToBeSwitchedULList_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
S1ap_E_RABToBeSwitchedULList_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	S1ap_E_RABToBeSwitchedULList_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
S1ap_E_RABToBeSwitchedULList_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	S1ap_E_RABToBeSwitchedULList_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
S1ap_E_RABToBeSwitchedULList_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	S1ap_E_RABToBeSwitchedULList_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
S1ap_E_RABToBeSwitchedULList_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	S1ap_E_RABToBeSwitchedULList_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
S1ap_E_RABToBeSwitchedULList_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	S1ap_E_RABToBeSwitchedULList_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

asn_dec_rval_t
S1ap_E_RABToBeSwitchedULList_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
	S1ap_E_RABToBeSwitchedULList_1_inherit_TYPE_descriptor(td);
	return td->uper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

asn_enc_rval_t
S1ap_E_RABToBeSwitchedULList_encode_uper(asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints,
		void *structure, asn_per_outp_t *per_out) {
	S1ap_E_RABToBeSwitchedULList_1_inherit_TYPE_descriptor(td);
	return td->uper_encoder(td, constraints, structure, per_out);
}

asn_enc_rval_t
S1ap_E_RABToBeSwitchedULList_encode_aper(asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints,
		void *structure, asn_per_outp_t *per_out) {
	S1ap_E_RABToBeSwitchedULList_1_inherit_TYPE_descriptor(td);
	return td->aper_encoder(td, constraints, structure, per_out);
}

asn_dec_rval_t
S1ap_E_RABToBeSwitchedULList_decode_aper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
	S1ap_E_RABToBeSwitchedULList_1_inherit_TYPE_descriptor(td);
	return td->aper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

static const ber_tlv_tag_t asn_DEF_S1ap_E_RABToBeSwitchedULList_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
asn_TYPE_descriptor_t asn_DEF_S1ap_E_RABToBeSwitchedULList = {
	"S1ap-E-RABToBeSwitchedULList",
	"S1ap-E-RABToBeSwitchedULList",
	S1ap_E_RABToBeSwitchedULList_free,
	S1ap_E_RABToBeSwitchedULList_print,
	S1ap_E_RABToBeSwitchedULList_constraint,
	S1ap_E_RABToBeSwitchedULList_decode_ber,
	S1ap_E_RABToBeSwitchedULList_encode_der,
	S1ap_E_RABToBeSwitchedULList_decode_xer,
	S1ap_E_RABToBeSwitchedULList_encode_xer,
	S1ap_E_RABToBeSwitchedULList_decode_uper,
	S1ap_E_RABToBeSwitchedULList_encode_uper,
	S1ap_E_RABToBeSwitchedULList_decode_aper,
	S1ap_E_RABToBeSwitchedULList_encode_aper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_S1ap_E_RABToBeSwitchedULList_tags_1,
	sizeof(asn_DEF_S1ap_E_RABToBeSwitchedULList_tags_1)
		/sizeof(asn_DEF_S1ap_E_RABToBeSwitchedULList_tags_1[0]), /* 1 */
	asn_DEF_S1ap_E_RABToBeSwitchedULList_tags_1,	/* Same as above */
	sizeof(asn_DEF_S1ap_E_RABToBeSwitchedULList_tags_1)
		/sizeof(asn_DEF_S1ap_E_RABToBeSwitchedULList_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* Defined elsewhere */
	0	/* No specifics */
};

