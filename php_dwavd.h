/*
 * Copyright (c) Doctor Web, 2003-2013
 *
 * Following code is the property of Doctor Web, Ltd.
 * Dr.Web is a registered trademark of Doctor Web, Ltd.
 *
 * http://www.drweb.com
 * http://www.av-desk.com
 *
 */

#ifndef PHP_DWAVD_H
#define PHP_DWAVD_H

#include "zend.h"
#include "php.h"
#include "php_utils.h"

extern zend_module_entry dwavd_module_entry;
#define phpext_dwavd_ptr &dwavd_module_entry

#ifdef PHP_WIN32
#	define PHP_DWAVD_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_DWAVD_API __attribute__ ((visibility("default")))
#else
#	define PHP_DWAVD_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define DWAVD_EXTENSION_VERSION "1.1.0"

/**
 * Числовая версия в формате:
 *
 * 	0xXXYYZZ
 *
 * Где XX - Мажорная часть версии, YY - Минорная часть версии, XX - Патч часть версии.
 * 
 */
#define DWAVD_EXTENSION_VERSION_NUM 0x010100

#include <dwavdapi/avdesk.h>
#include <dwavdapi/groups.h>
#include <dwavdapi/tariffs.h>
#include <dwavdapi/stations.h>
#include <dwavdapi/server.h>

#define LE_DWAVD_NAME                           "Dr.Web AV-Desk handle"
#define LE_DWAVD_SRV_NAME                       "Dr.Web AV-Desk server"
#define LE_DWAVD_SRV_KEY_NAME                   "Dr.Web AV-Desk server key"
#define LE_DWAVD_ADM_NAME                       "Dr.Web AV-Desk administrator"
#define LE_DWAVD_GRP_NAME                       "Dr.Web AV-Desk group"
#define LE_DWAVD_GRP_STATS_NAME                 "Dr.Web AV-Desk group statistics"
#define LE_DWAVD_TRF_NAME                       "Dr.Web AV-Desk tariff"
#define LE_DWAVD_COMPONENTS_LST_NAME            "Dr.Web AV-Desk components list"
#define LE_DWAVD_COMPONENT_NAME                 "Dr.Web AV-Desk component"
#define LE_DWAVD_COMPONENTS_RUNNING_LST_NAME    "Dr.Web AV-Desk running components list"
#define LE_DWAVD_COMPONENT_RUNNING_NAME         "Dr.Web AV-Desk running component"
#define LE_DWAVD_COMPONENTS_INSTALLED_LST_NAME  "Dr.Web AV-Desk running components list"
#define LE_DWAVD_COMPONENT_INSTALLED_NAME       "Dr.Web AV-Desk installed component"
#define LE_DWAVD_RIGHTS_LST_NAME                "Dr.Web AV-Desk rights list"
#define LE_DWAVD_RIGHT_NAME                     "Dr.Web AV-Desk right"
#define LE_DWAVD_BASES_LST_NAME                 "Dr.Web AV-Desk bases list"
#define LE_DWAVD_BASE_NAME                      "Dr.Web AV-Desk base"
#define LE_DWAVD_MODULES_LST_NAME               "Dr.Web AV-Desk modules list"
#define LE_DWAVD_MODULE_NAME                    "Dr.Web AV-Desk module"
#define LE_DWAVD_ST_NAME                        "Dr.Web AV-Desk station"
#define LE_DWAVD_TRF_LST_NAME                   "Dr.Web AV-Desk tariffs list"
#define LE_DWAVD_GRP_LST_NAME                   "Dr.Web AV-Desk groups list"
#define LE_DWAVD_ADM_LST_NAME                   "Dr.Web AV-Desk administrators list"
#define LE_DWAVD_SRV_STATS_NAME                 "Dr.Web AV-Desk server statistics"       
#define LE_DWAVD_STATS_SCANS_NAME               "Dr.Web AV-Desk scans statistics"
#define LE_DWAVD_STATS_INFECTIONS_NAME          "Dr.Web AV-Desk infections statistics"
#define LE_DWAVD_STATIONS_STATE_NAME            "Dr.Web AV-Desk stations state"
#define LE_DWAVD_STATS_TRAFFIC_NAME             "Dr.Web AV-Desk traffic statistics"
#define LE_DWAVD_VIRUSES_LST_NAME               "Dr.Web AV-Desk viruses list"
#define LE_DWAVD_VIRUS_NAME                     "Dr.Web AV-Desk virus"
#define LE_DWAVD_INFCD_OBJS_LST_NAME            "Dr.Web AV-Desk infected objects list"
#define LE_DWAVD_INFCD_OBJ_NAME                 "Dr.Web AV-Desk infected object"
#define LE_DWAVD_ST_STATS_NAME                  "Dr.Web AV-Desk station statistics"


PHP_MINIT_FUNCTION(dwavd);
PHP_MSHUTDOWN_FUNCTION(dwavd);
PHP_RINIT_FUNCTION(dwavd);
PHP_RSHUTDOWN_FUNCTION(dwavd);
PHP_MINFO_FUNCTION(dwavd);

PHP_FUNCTION(dwavd_init);
PHP_FUNCTION(dwavd_free);
PHP_FUNCTION(dwavd_version);
PHP_FUNCTION(dwavd_error);
PHP_FUNCTION(dwavd_lib_version);
PHP_FUNCTION(dwavd_set_host);
PHP_FUNCTION(dwavd_set_port);
PHP_FUNCTION(dwavd_set_login);
PHP_FUNCTION(dwavd_set_password);
PHP_FUNCTION(dwavd_set_timeout);
PHP_FUNCTION(dwavd_set_user_agent);
PHP_FUNCTION(dwavd_set_ssl_crt);
PHP_FUNCTION(dwavd_srv_get_info);
PHP_FUNCTION(dwavd_srv_free);
PHP_FUNCTION(dwavd_srv_array);
PHP_FUNCTION(dwavd_srv);
PHP_FUNCTION(dwavd_srv_key_get_info);
PHP_FUNCTION(dwavd_srv_key_free);
PHP_FUNCTION(dwavd_srv_key_array);
PHP_FUNCTION(dwavd_srv_key);
PHP_FUNCTION(dwavd_srv_run_task);
PHP_FUNCTION(dwavd_srv_get_stats);
PHP_FUNCTION(dwavd_srv_stats_free);
PHP_FUNCTION(dwavd_srv_stats);
PHP_FUNCTION(dwavd_srv_stats_array);
PHP_FUNCTION(dwavd_adm_get_info);
PHP_FUNCTION(dwavd_adm_get_list);
PHP_FUNCTION(dwavd_adm_free);
PHP_FUNCTION(dwavd_adm_list_free);
PHP_FUNCTION(dwavd_adm_array);
PHP_FUNCTION(dwavd_adm);
PHP_FUNCTION(dwavd_adm_set);
PHP_FUNCTION(dwavd_adm_set_array);
PHP_FUNCTION(dwavd_adm_add);
PHP_FUNCTION(dwavd_adm_change);
PHP_FUNCTION(dwavd_adm_delete);
PHP_FUNCTION(dwavd_grp_get_info);
PHP_FUNCTION(dwavd_grp_get_list);
PHP_FUNCTION(dwavd_grp_free);
PHP_FUNCTION(dwavd_grp_list_free);
PHP_FUNCTION(dwavd_grp);
PHP_FUNCTION(dwavd_grp_get_stats);
PHP_FUNCTION(dwavd_grp_stats_free); 
PHP_FUNCTION(dwavd_grp_stats);
PHP_FUNCTION(dwavd_grp_stats_array);
PHP_FUNCTION(dwavd_grp_set);
PHP_FUNCTION(dwavd_grp_set_array);
PHP_FUNCTION(dwavd_grp_add);
PHP_FUNCTION(dwavd_grp_change);
PHP_FUNCTION(dwavd_grp_array);
PHP_FUNCTION(dwavd_grp_delete);
PHP_FUNCTION(dwavd_trf_get_info);
PHP_FUNCTION(dwavd_trf_get_list);
PHP_FUNCTION(dwavd_trf_free);
PHP_FUNCTION(dwavd_trf_list_free);
PHP_FUNCTION(dwavd_trf);
PHP_FUNCTION(dwavd_trf_set);
PHP_FUNCTION(dwavd_trf_set_array);
PHP_FUNCTION(dwavd_trf_set_component);
PHP_FUNCTION(dwavd_trf_add);
PHP_FUNCTION(dwavd_trf_change);
PHP_FUNCTION(dwavd_trf_array);
PHP_FUNCTION(dwavd_trf_delete);
PHP_FUNCTION(dwavd_list_next);
PHP_FUNCTION(dwavd_list_prev);
PHP_FUNCTION(dwavd_list_rewind);
PHP_FUNCTION(dwavd_list_current);
PHP_FUNCTION(dwavd_list_array);
PHP_FUNCTION(dwavd_component);
PHP_FUNCTION(dwavd_component_array);
PHP_FUNCTION(dwavd_right_array);
PHP_FUNCTION(dwavd_right);
PHP_FUNCTION(dwavd_st_get_info);
PHP_FUNCTION(dwavd_st_free);
PHP_FUNCTION(dwavd_st_array);
PHP_FUNCTION(dwavd_st);
PHP_FUNCTION(dwavd_st_stats);
PHP_FUNCTION(dwavd_st_get_stats);
PHP_FUNCTION(dwavd_st_stats_free);
PHP_FUNCTION(dwavd_st_stats_array);
PHP_FUNCTION(dwavd_st_set);
PHP_FUNCTION(dwavd_st_set_array);
PHP_FUNCTION(dwavd_st_add);
PHP_FUNCTION(dwavd_st_change);
PHP_FUNCTION(dwavd_st_delete);
PHP_FUNCTION(dwavd_base);
PHP_FUNCTION(dwavd_base_array);
PHP_FUNCTION(dwavd_module);
PHP_FUNCTION(dwavd_module_array);
PHP_FUNCTION(dwavd_component_installed_array);
PHP_FUNCTION(dwavd_component_installed);
PHP_FUNCTION(dwavd_component_running_array);
PHP_FUNCTION(dwavd_component_running);
PHP_FUNCTION(dwavd_stats_scans);
PHP_FUNCTION(dwavd_stats_scans_array);
PHP_FUNCTION(dwavd_stats_infcd_array);
PHP_FUNCTION(dwavd_stats_infcd);
PHP_FUNCTION(dwavd_stats_sts_state);
PHP_FUNCTION(dwavd_stats_sts_state_array);
PHP_FUNCTION(dwavd_stats_traffic);
PHP_FUNCTION(dwavd_stats_traffic_array);
PHP_FUNCTION(dwavd_virus);
PHP_FUNCTION(dwavd_virus_array);
PHP_FUNCTION(dwavd_infcd_obj);
PHP_FUNCTION(dwavd_infcd_obj_array);
PHP_FUNCTION(dwavd_grp_send_message);
PHP_FUNCTION(dwavd_st_send_message);
PHP_FUNCTION(dwavd_grp_init); 
PHP_FUNCTION(dwavd_trf_init); 
PHP_FUNCTION(dwavd_adm_init); 
PHP_FUNCTION(dwavd_st_init);

typedef struct _dwavd_opt {
    char *opt;
    int falg;
} _dwavd_opt;

ZEND_BEGIN_MODULE_GLOBALS(dwavd)
    long avdesk_port;
    char *avdesk_host;
    char *avdesk_login;
    char *avdesk_password;
    char *avdesk_crt;
ZEND_END_MODULE_GLOBALS(dwavd)

/* In every utility function you add that needs to use variables 
   in php_dwavd_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as DWAVD_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
        
enum {
    DWAVD_SRV_ID = 1,
    DWAVD_SRV_OS,
    DWAVD_SRV_HOST,
    DWAVD_SRV_PLATFORM,
    DWAVD_SRV_VERSION,
    DWAVD_SRV_API_VERSION,
    DWAVD_SRV_API_BUILD,
    DWAVD_SRV_STATIONS_TOTAL,
    DWAVD_SRV_STATIONS_LICENSED,
    DWAVD_SRV_STATIONS_AVAILABLE,
    DWAVD_SRV_GROUPS_TOTAL,
    DWAVD_SRV_TARIFFS_TOTAL,
    DWAVD_SRV_GROUPS_CUSTOM,
    DWAVD_SRV_GROUPS_SYSTEM,
    DWAVD_SRV_UPTIME,
    DWAVD_SRVKEY_ID,
    DWAVD_SRVKEY_MD5,
    DWAVD_SRVKEY_SN,
    DWAVD_SRVKEY_DEALER,
    DWAVD_SRVKEY_DEALER_NAME,
    DWAVD_SRVKEY_USER,
    DWAVD_SRVKEY_USER_NAME,
    DWAVD_SRVKEY_SERVERS,
    DWAVD_SRVKEY_ANTISPAM,
    DWAVD_SRVKEY_CLIENTS,
    DWAVD_SRVKEY_PRODUCTS,
    DWAVD_SRVKEY_ACTIVATED,
    DWAVD_SRVKEY_CREATED,
    DWAVD_SRVKEY_EXPIRES,
    DWAVD_SRV_STATS_TARIFFS_TOTAL,
    DWAVD_SRV_STATS_GROUPS_TOTAL,
    DWAVD_SRV_STATS_GROUPS_CUSTOM,
    DWAVD_SRV_STATS_GROUPS_SYSTEM,
    DWAVD_SRV_STATS_STATIONS_TOTAL,
    DWAVD_SRV_STATS_STATIONS_STATE,
    DWAVD_SRV_STATS_SCANS,
    DWAVD_SRV_STATS_INFECTIONS,
    DWAVD_SRV_STATS_VIRUSES,
    DWAVD_SRV_STATS_TRAFFIC,
    DWAVD_ADM_ID,
    DWAVD_ADM_CREATED,
    DWAVD_ADM_MODIFIED,
    DWAVD_ADM_NAME,
    DWAVD_ADM_LAST_NAME,
    DWAVD_ADM_MIDDLE_NAME,
    DWAVD_ADM_LOGIN,
    DWAVD_ADM_PASSWORD,
    DWAVD_ADM_DESCRIPTION,
    DWAVD_ADM_GROUPS,
    DWAVD_ADM_GROUPS_COUNT,
    DWAVD_ADM_TYPE,
    DWAVD_GRP_ID,
    DWAVD_GRP_CREATED,
    DWAVD_GRP_MODIFIED,
    DWAVD_GRP_NAME,
    DWAVD_GRP_DESCRIPTION,
    DWAVD_GRP_CHILD_GROUPS,
    DWAVD_GRP_CHILD_GROUPS_COUNT,
    DWAVD_GRP_EMAILS,
    DWAVD_GRP_EMAILS_COUNT,
    DWAVD_GRP_ADMINS,
    DWAVD_GRP_ADMINS_COUNT,
    DWAVD_GRP_STATIONS,
    DWAVD_GRP_STATIONS_COUNT,
    DWAVD_GRP_TYPE,
    DWAVD_GRP_PARENT_ID,
    DWAVD_GRP_RIGHTS,
    DWAVD_ADM_DEL_FROM_GROUPS,
    DWAVD_GRP_STATS_STATIONS_TOTAL,
    DWAVD_GRP_STATS_STATIONS_STATE,
    DWAVD_GRP_STATS_SCANS,
    DWAVD_GRP_STATS_INFECTIONS,
    DWAVD_GRP_STATS_VIRUSES,  
    DWAVD_GRP_DEL_EMAILS,
    DWAVD_TRF_ID,
    DWAVD_TRF_CREATED,
    DWAVD_TRF_MODIFIED,
    DWAVD_TRF_NAME,
    DWAVD_TRF_DESCRIPTION,
    DWAVD_TRF_CHILD_GROUPS,
    DWAVD_TRF_CHILD_GROUPS_COUNT,
    DWAVD_TRF_STATIONS,
    DWAVD_TRF_STATIONS_COUNT,
    DWAVD_TRF_TYPE,
    DWAVD_TRF_GRACE_PERIOD,
    DWAVD_TRF_COMPONENTS,
    DWAVD_TRF_RIGHTS,
    DWAVD_TRF_PARENT_ID,
    DWAVD_CMP_NAME,
    DWAVD_CMP_CODE,
    DWAVD_CMP_VALUE,
    DWAVD_CMP_INHERITED_GROUP_ID,
    DWAVD_RHT_CODE,
    DWAVD_RHT_VALUE,
    DWAVD_RHT_INHERITED_GROUP_ID,
    DWAVD_RHT_NAME,
    DWAVD_CMP_INSTD_NAME,
    DWAVD_CMP_INSTD_INSTALLED,
    DWAVD_CMP_INSTD_SERVER,
    DWAVD_CMP_INSTD_PATH,
    DWAVD_CMP_RUN_NAME,
    DWAVD_CMP_RUN_STARTED,
    DWAVD_CMP_RUN_TYPE,
    DWAVD_CMP_RUN_USER,
    DWAVD_CMP_RUN_PARAMS,
    DWAVD_BASE_FILE,
    DWAVD_BASE_VERSION,
    DWAVD_BASE_VIRUSES,
    DWAVD_BASE_CREATED,
    DWAVD_MOD_FILE,
    DWAVD_MOD_VERSION,
    DWAVD_MOD_CREATED,
    DWAVD_MOD_MODIFIED,
    DWAVD_MOD_HASH,
    DWAVD_MOD_SIZE,
    DWAVD_MOD_NAME,
    DWAVD_ST_OS,
    DWAVD_ST_OS_NAME,
    DWAVD_ST_STATE,
    DWAVD_ST_COUNTRY,
    DWAVD_ST_COUNTRY_NAME,
    DWAVD_ST_COUNTRY_CODE,
    DWAVD_ST_GRACE_PERIOD,
    DWAVD_ST_LONGITUDE,
    DWAVD_ST_LATITUDE,
    DWAVD_ST_CREATED,
    DWAVD_ST_MODIFIED,
    DWAVD_ST_EXPIRES,
    DWAVD_ST_CITY,
    DWAVD_ST_FLOOR,
    DWAVD_ST_URL,
    DWAVD_ST_PASSWORD,
    DWAVD_ST_DESCRIPTION,
    DWAVD_ST_ID,
    DWAVD_ST_ROOM,
    DWAVD_ST_NAME,
    DWAVD_ST_STREET,
    DWAVD_ST_PROVINCE,
    DWAVD_ST_DEPARTMENT,
    DWAVD_ST_TARIFF_ID,
    DWAVD_ST_ORGANIZATION,
    DWAVD_ST_PARENT_ID,
    DWAVD_ST_EMAILS,
    DWAVD_ST_EMAILS_COUNT,
    DWAVD_ST_DEL_EMAILS,
    DWAVD_ST_GROUPS,
    DWAVD_ST_GROUPS_COUNT,
    DWAVD_ST_COMPONENTS,
    DWAVD_ST_BASES,
    DWAVD_ST_MODULES,
    DWAVD_ST_RIGHTS,
    DWAVD_ST_COMPONENTS_RUN,
    DWAVD_ST_COMPONENTS_INSTD,
    DWAVD_ST_BLOCK_BEGINS,
    DWAVD_ST_BLOCK_ENDS,
    DWAVD_ST_LASTSEEN_TIME,
    DWAVD_ST_LASTSEEN_ADDR,            
    DWAVD_ST_STATE_ONLINE,
    DWAVD_ST_STATE_DEINSTALLED,
    DWAVD_ST_STATE_BLOCKED,
    DWAVD_ST_STATE_EXPIRED,
    DWAVD_ST_STATE_OFFLINE,
    DWAVD_ST_STATE_ACTIVATED,
    DWAVD_ST_STATE_UNACTIVATED,
    DWAVD_ST_STATE_TOTAL,     
    DWAVD_ST_STATS_SCANS,
    DWAVD_ST_STATS_INFECTIONS,
    DWAVD_ST_STATS_VIRUSES,    
    DWAVD_SCANS_SIZE,
    DWAVD_SCANS_FILES,
    DWAVD_SCANS_DELETED,
    DWAVD_SCANS_MOVED,
    DWAVD_SCANS_CURED,
    DWAVD_SCANS_ERRORS,
    DWAVD_SCANS_INFECTED,
    DWAVD_SCANS_RENAMED,
    DWAVD_SCANS_LOCKED,            
    DWAVD_INFCD_DELETED,
    DWAVD_INFCD_CURED,
    DWAVD_INFCD_MOVED,
    DWAVD_INFCD_INCURABLE,
    DWAVD_INFCD_LOCKED,
    DWAVD_INFCD_RENAMED,
    DWAVD_INFCD_ERRORS,
    DWAVD_INFCD_IGNORED,
    DWAVD_INFCD_TOTAL,
    DWAVD_TRAFFIC_TOTAL,
    DWAVD_TRAFFIC_IN,
    DWAVD_TRAFFIC_OUT,
    DWAVD_VIRUS_NAME,
    DWAVD_VIRUS_OBJECTS,
    DWAVD_VIRUS_OBJECTS_COUNT,
    DWAVD_INFCD_OBJ_TYPE,
    DWAVD_INFCD_OBJ_CURE,
    DWAVD_INFCD_OBJ_TYPE_CODE,
    DWAVD_INFCD_OBJ_CURE_CODE,
    DWAVD_INFCD_OBJ_INFECTION_TYPE,
    DWAVD_INFCD_OBJ_INFECTION_TYPE_CODE,
    DWAVD_INFCD_OBJ_ORIGINATOR,
    DWAVD_INFCD_OBJ_ORIGINATOR_CODE,
    DWAVD_INFCD_OBJ_PATH,
    DWAVD_INFCD_OBJ_OWNER,
    DWAVD_INFCD_OBJ_STATION_ID,
    DWAVD_INFCD_OBJ_USERNAME
};

#define DWAVD_REGISTER_LONG_CONSTANT(def) \
        REGISTER_LONG_CONSTANT(#def, def, CONST_CS | CONST_PERSISTENT);

#ifdef ZTS
#define DWAVD_G(v) TSRMG(dwavd_globals_id, zend_dwavd_globals *, v)
#else
#define DWAVD_G(v) (dwavd_globals.v)
#endif
        
#define DWAVD_INVALID_RESOURCE  \
        zend_error(E_WARNING, "Invalid resource provided"); \

#define DWAVD_UNKNOWN_OPTION(opt) \
        do { \
           convert_to_string(opt) \
           _dwavd_error(E_WARNING, "Unknown option: `%s'", Z_STRVAL_P(opt)); \
           RETURN_FALSE \
        } while(0);
        
#define DWAVD_INVALID_RESOURCE_WITH_RETURN_NULL \
   do { \
        DWAVD_INVALID_RESOURCE \
        RETURN_NULL(); \
   } while(0);        
                  
#define DWAVD_FETCH_HANDLE(handle, res_id, res_type, res_found_type) \
    handle = (dwavdapi_handle *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        return; \
    }      
        
#define DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, res_id, res_type, res_found_type) \
    handle = (dwavdapi_handle *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }          

#define DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, res_id, res_type, res_found_type) \
    handle = (dwavdapi_handle *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_NULL(); \
    }               

#define DWAVD_CHECK_PARAM_COUNT(count) \
    if(count < ZEND_NUM_ARGS() || count > ZEND_NUM_ARGS()) { \
        _dwavd_error(E_WARNING, "Expects exactly %d parameters, %d given", count, ZEND_NUM_ARGS()); \
    }
      
#define DWAVD_FETCH_SRV_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_server *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }      

#define DWAVD_FETCH_SRV_STATS_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_server_statistics *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }      

#define DWAVD_FETCH_SRVKEY_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_server_key *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }

#define DWAVD_ADD_ASSOC_STRING_OR_NULL(array, key, value) \
       if(NULL == value) { \
           add_assoc_null(array, key); \
       } else { \
           add_assoc_string(array, key, value, 1); \
       }

#define DWAVD_RETURN_STRING_OR_NULL(value) \
        if(NULL == value) { \
            RETURN_NULL(); \
        } else { \
            RETURN_STRING(value, 1);\
        }

#define DWAVD_FETCH_ADM_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_admin *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }      

#define DWAVD_FETCH_ADM_LST_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_list *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }     

#define DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_group *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }  

#define DWAVD_FETCH_GRP_LST_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_list *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    } 

#define DWAVD_FETCH_GRP_STATS_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_group_statistics *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }    
        
#define DWAVD_FETCH_LST_RES_WITH_RETURN_FALSE(res, res_id, res_found_type) \
    res = (dwavdapi_list *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }      
        
#define DWAVD_FETCH_LST_RES_WITH_RETURN_NULL(res, res_id, res_found_type) \
    res = (dwavdapi_list *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_NULL(); \
    } 

#define DWAVD_FETCH_LST_RES(res, res_id, res_found_type) \
    res = (dwavdapi_list *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res) { \
        DWAVD_INVALID_RESOURCE \
        return; \
    } 

#define DWAVD_LST_DATA(data, lst) \
        data = dwavdapi_list_current_data(lst); \
        if(NULL == data) { \
            RETURN_NULL(); \
        }

#define DWAVD_FETCH_CMPS_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_component *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }     

#define DWAVD_FETCH_CMP_INSTD_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_installed_component *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    } 

#define DWAVD_FETCH_CMP_RUN_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_running_component *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    } 

#define DWAVD_FETCH_CMP_RUN_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_running_component *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    } 

#define DWAVD_FETCH_SCANS_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_statistics_scans *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    } 

#define DWAVD_FETCH_STATES_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_statistics_stations_state *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    } 

#define DWAVD_FETCH_VIRUS_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_virus *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    } 

#define DWAVD_FETCH_INFCD_OBJ_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_infected_object *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    } 

#define DWAVD_FETCH_TRAFFIC_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_statistics_traffic *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    } 

#define DWAVD_FETCH_INFCD_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_statistics_infections *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    } 

#define DWAVD_FETCH_RHT_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_right *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }       

#define DWAVD_FETCH_BASE_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_base *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }      

#define DWAVD_FETCH_MOD_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_module *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }      

#define DWAVD_FETCH_ST_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_station *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }      
        
#define DWAVD_FETCH_ST_RES_WITH_RETURN_NULL(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_station *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_NULL(); \
    }   

#define DWAVD_FETCH_ST_STATS_RES_WITH_RETURN_FALSE(res, res_id, res_type, res_found_type) \
    res = (dwavdapi_station_statistics *) zend_list_find(res_id, &res_found_type); \
    if (NULL == res || res_found_type != res_type) { \
        DWAVD_INVALID_RESOURCE \
        RETURN_FALSE \
    }   

#define DWAVD_OPT_TO_FLAG(fl, opt, arr) \
        do { \
            if(Z_TYPE_P(opt) == IS_STRING) { \
                fl = _dwavd_opt_to_flag(Z_STRVAL_P(opt), arr, sizeof(arr)/sizeof(arr[0])); \
                if(-1 == fl) { \
                   _dwavd_error(E_WARNING, "Unknown option: `%s'", Z_STRVAL_P(opt)); \
                   RETURN_FALSE \
                }\
            } else if (Z_TYPE_P(opt) == IS_LONG) { \
               fl = Z_LVAL_P(opt); \
            } else { \
               _dwavd_error(E_WARNING, "Expected string or number, got %s", _dwavd_var_type(opt));\
               RETURN_FALSE \
            } \
        } while(0);

#define DWAVD_ADM_SET_STRING(res, lower_name, value) \
        if(Z_TYPE_P(val) == IS_NULL) { \
                if(DWAVDAPI_SUCCESS == dwavdapi_admin_set_##lower_name(res, NULL)) { \
                        return 0; \
                } \
                return 1; \
        } else if(Z_TYPE_P(val) == IS_STRING) { \
                if(DWAVDAPI_SUCCESS == dwavdapi_admin_set_##lower_name(res, Z_STRVAL_P(value))) { \
                       return 0; \
                } \
                return 1; \
         } else { \
               _dwavd_error(E_WARNING, "Expected string, got %s", _dwavd_var_type(val)); \
               return 1; \
         }

#define DWAVD_GRP_SET_STRING(res, lower_name, value) \
        if(Z_TYPE_P(val) == IS_NULL) { \
                if(DWAVDAPI_SUCCESS == dwavdapi_group_set_##lower_name(res, NULL)) { \
                        return 0; \
                } \
                return 1; \
        } else if(Z_TYPE_P(val) == IS_STRING) { \
                if(DWAVDAPI_SUCCESS == dwavdapi_group_set_##lower_name(res, Z_STRVAL_P(value))) { \
                        return 0; \
                } \
                return 1; \
        } else { \
                _dwavd_error(E_WARNING, "Expected string, got %s", _dwavd_var_type(val)); \
               return 1; \
        }

#define DWAVD_GRP_SET_LONG(res, lower_name, value) \
        if(Z_TYPE_P(val) == IS_NULL) { \
                if(DWAVDAPI_SUCCESS == dwavdapi_group_set_##lower_name(res, 0)) { \
                        return 0; \
                } \
                return 1; \
        } else if (Z_TYPE_P(val) == IS_LONG) { \
                convert_to_ex_master(&val, long, LONG) \
                if(DWAVDAPI_SUCCESS == dwavdapi_group_set_##lower_name(res, Z_LVAL_P(value))) { \
                        return 0; \
                } \
                return 1; \
        } else {\
                _dwavd_error(E_WARNING, "Expected integer, got %s", _dwavd_var_type(val)); \
               return 1; \
        }

#define DWAVD_ST_SET_STRING(res, lower_name, value) \
        if(Z_TYPE_P(val) == IS_NULL) { \
                if(DWAVDAPI_SUCCESS == dwavdapi_station_set_##lower_name(res, NULL)) { \
                        return 0; \
                } \
                return 1; \
        } else if(Z_TYPE_P(val) == IS_STRING) { \
                if(DWAVDAPI_SUCCESS == dwavdapi_station_set_##lower_name(res, Z_STRVAL_P(value))) { \
                        return 0; \
                } \
                return 1; \
        } else { \
                _dwavd_error(E_WARNING, "Expected string, got %s", _dwavd_var_type(val)); \
               return 1; \
        }

#define DWAVD_ST_SET_LONG(res, lower_name, value) \
        if(Z_TYPE_P(val) == IS_NULL) { \
                if(DWAVDAPI_SUCCESS == dwavdapi_station_set_##lower_name(res, 0)) { \
                        return 0; \
                } \
                return 1; \
        } else if(Z_TYPE_P(val) == IS_LONG) { \
                if(DWAVDAPI_SUCCESS == dwavdapi_station_set_##lower_name(res, Z_LVAL_P(value))) { \
                        return 0; \
                } \
                return 1; \
        } else {\
                _dwavd_error(E_WARNING, "Expected integer, got %s", _dwavd_var_type(val)); \
               return 1; \
        }

#define DWAVD_VALUE_CANT_BE_EMPTY(opt, val) \
        if(Z_TYPE_P(val) == IS_STRING && Z_STRLEN_P(val) == 0) {\
                _dwavd_error(E_WARNING, "Value of option `%s' cannot be empty",  Z_STRVAL_P(opt)); \
                RETURN_FALSE \
        }

#define DWAVD_EXPECTED_ARRAY(val) \
        if(Z_TYPE_P(val) != IS_ARRAY) {\
                _dwavd_error(E_WARNING, "Excepted array, got %s", _dwavd_var_type(val)); \
                RETURN_FALSE \
        }

#define DWAVD_EXPECTED_ARRAY_WITH_RET(val, ret) \
        if(Z_TYPE_P(val) != IS_ARRAY) {\
                _dwavd_error(E_WARNING, "Excepted array, got %s", _dwavd_var_type(val)); \
                return ret; \
        }

#define DWAVD_ARRAY_EMPTY(arr) \
        if(0 == zend_hash_num_elements(arr)) { \
                _dwavd_error(E_WARNING, "Array not contains any elements"); \
                RETURN_FALSE \
        }

#define DWAVD_ARRAY_EMPTY_WITH_RET(arr, ret) \
        if(0 == zend_hash_num_elements(arr)) { \
                _dwavd_error(E_WARNING, "Array not contains any elements"); \
                return ret; \
        }

#define DWAVD_CHECK_TIME(t, name) \
    if(0 > t) { \
        _dwavd_error(E_WARNING, "Incorrect time value of argument `%s'", #name); \
        RETURN_NULL(); \
    } \
                
#endif	/* PHP_DWAVDESK_H */


