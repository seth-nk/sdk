#ifndef _NKCAT_H
#define _NKCAT_H

#define NKCAT_ERROR_SUCCESS		0
#define NKCAT_ERROR_OPEN_FAILD		-1
#define NKCAT_ERROR_METADATA_READ	-2
#define NKCAT_ERROR_DATA_INVALID	-3
#define NKCAT_ERROR_NOPAPNAME		-4
#define NKCAT_ERROR_NOMEM		-5
#define NKCAT_ERROR_FILE_SEEK		-6
#define NKCAT_ERROR_DATE_READ		-7

int seth_nkpin_get_from_file(const char *file, const char *username, time_t timestamp, char **pap_username);

#endif /* _NKCAT_H */
