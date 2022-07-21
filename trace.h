/**
 * @file tace.h
 * @brief Header file for trace definition.
 *
*/

#ifndef _DBG_PRINT_H_
#define _DBG_PRINT_H_

#ifdef ENABLE_TRACES
#define debug_print_dbg(fmt, args...) fprintf(stdout, "\033[0;32m[DBG]: %s(%d): " fmt "\033[0m\n", __FUNCTION__, __LINE__, ##args)
#define debug_print_wrn(fmt, args...) fprintf(stdout, "\033[0;33m[WRN]: %s(%d): " fmt "\033[0m\n", __FUNCTION__, __LINE__, ##args)
#define debug_print_mqtt(fmt, args...) fprintf(stdout, "\033[0;36m[MQTT]: %s(%d): " fmt "\033[0m\n", __FUNCTION__, __LINE__, ##args)
#endif
#ifndef DISABLE_ERROR_TRACE
#define debug_print_err(fmt, args...) fprintf(stderr, "\033[0;31m[ERR]: %s(%d): " fmt "\033[0m\n", __FUNCTION__, __LINE__, ##args)
#endif

#ifdef ENABLE_TRACES
#define print_dbg debug_print_dbg
#define print_wrn debug_print_wrn
#define print_mqtt debug_print_mqtt
#else
#define print_dbg
#define print_wrn
#endif

/* Error trace must be always shown */
#ifndef DISABLE_ERROR_TRACE
#define print_err debug_print_err
#else
#define print_err
#endif

#endif /* _DBG_PRINT_H_ */
