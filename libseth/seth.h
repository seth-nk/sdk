#ifndef _SETH_H_
#define _SETH_H_

#include <inttypes.h>
#include <sys/types.h>

#define __PACKSTRUCT __attribute__((packed))


extern const char *libseth_application_sdk_version;
void libseth_winnt_startup(const char *version);
int seth_application_main(int argc, char *argv[]);

#define LIBSETH_SDK_VERSION	"0.4.1"
#define LIBSETH_APPLICATION(name) \
const char *libseth_application_sdk_version = LIBSETH_SDK_VERSION; \
const char *libseth_application_name = name;

#if defined(LIBSETH_APPLICATION_PORTABLE_FOR_WINDOWS) && defined(_WIN32)
int main(int argc, char *argv[]) {
	libseth_winnt_startup(libseth_application_sdk_version);
	return seth_application_main(argc, argv);
}
#endif
#if defined(LIBSETH_APPLICATION_PORTABLE_FOR_POSIX) && !defined(_WIN32)
#define seth_application_main main
#endif


#define SETH_HDR_MAGIC0 0x23
#define SETH_HDR_MAGIC1 0x33
#define SETH_HDR_VERSION 3


struct __PACKSTRUCT seth_v2_hdr {
	uint8_t magic0;			// 0x23
	uint8_t magic1;			// 0x33
	uint16_t version;		// seth format version, SETH_HDR_VERSION
	uint32_t life;			// the life of every nk-pins

	uint64_t timestamp;		// the first nk-pin timestamp

	uint32_t offset;		// the offset of first nk-pin
	uint32_t length;		// the nk-pin length

	uint16_t isp;			// isp, see SETH_NK_ISP_*
	uint16_t nkversion;		// nk version, see SETH_NK_VERSION_*
	uint16_t nkrevision;		// revision version, defined by the author
	uint8_t prefix0;		// first prefix
	uint8_t prefix1;		// second prefix

	uint8_t flags;			// see SETH_NK_FLAG_*
	char papname[47];		// PAP username
};

struct __PACKSTRUCT seth_v2_option_hdr {
	uint8_t version;		// seth option version, SETH_OPTION_HDR_VERSION
	uint8_t flags;			// see SETH_OPTION_FLAG_*
	uint16_t length;		// length of the option field, in bytes, dwords or qwords.
	uint32_t option_id;		// option ID, see SETH_OPTION_*
};

#define SETH_NK_ISP_UNSPEC	0	// unspecified
#define SETH_NK_ISP_CMCC_SD	1	// CMCC Shandong

#define SETH_NK_VERSION_INV	0	// invalid
#define SETH_NK_VERSION_2_5	1	// Netkeeper 2.5.0059
#define SETH_NK_VERSION_4_9	2	// Netkeeper 4.9.19.699
#define SETH_NK_VERSION_4_11	3	// Netkeeper 4.11.4.731
#define SETH_NK_VERSION_5	4	// Netkeeper 5.x

#define SETH_NK_VER_2_5_REVISION_SDPATCH1  0x101 // pathed Netkeeper 2.5.0059 for CMCC Shandong coexist with 4.9

#define SETH_NK_FLAG_PrivacyExtensionEnabled   1		// papname invalid
#define SETH_NK_FLAG_OptionFieldsAvailable     (1 << 1)		// option fields available


#define SETH_OPTION_HDR_VERSION	1

#define SETH_OPTION_FLAG_CRUCIAL	1		// this option is a crucial option. must not be ignored.
#define SETH_OPTION_FLAG_QUAD		(1 << 1)	// quad size of length (length << 2)
#define SETH_OPTION_FLAG_OCTO		(1 << 2)	// octo size of length (length << 3)


#define SETH_HDR_GET_SET_UINT_WEAK(field, type, notused) \
type seth_hdr_get_##field(struct seth_v2_hdr *hdr); \
void seth_hdr_set_##field(struct seth_v2_hdr *hdr, type field);

SETH_HDR_GET_SET_UINT_WEAK(version, uint16_t, 16)
SETH_HDR_GET_SET_UINT_WEAK(life, size_t, 32)
SETH_HDR_GET_SET_UINT_WEAK(timestamp, time_t, 64)
SETH_HDR_GET_SET_UINT_WEAK(offset, size_t, 32)
SETH_HDR_GET_SET_UINT_WEAK(length, size_t, 32)
SETH_HDR_GET_SET_UINT_WEAK(isp, uint16_t, 16)
SETH_HDR_GET_SET_UINT_WEAK(nkversion, uint16_t, 16)
SETH_HDR_GET_SET_UINT_WEAK(nkrevision, uint16_t, 16)
SETH_HDR_GET_SET_UINT_WEAK(prefix0, uint8_t, 8)
SETH_HDR_GET_SET_UINT_WEAK(prefix1, uint8_t, 8)
SETH_HDR_GET_SET_UINT_WEAK(flags, uint8_t, 8)

void seth_hdr_add_flag(struct seth_v2_hdr *hdr, uint8_t flag);

struct seth_v2_hdr *seth_hdr_new();
int seth_hdr_valid(struct seth_v2_hdr *hdr);
off_t seth_nkpin_offset(struct seth_v2_hdr *hdr, const time_t timestamp);

#endif // _SETH_H_
