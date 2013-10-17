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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <errno.h>

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <main/php.h>
#include <main/SAPI.h>
#include <Zend/zend.h>
#include <Zend/zend_globals.h>
#include <Zend/zend_API.h>
#include <ext/standard/info.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif /* HAVE_STDARG_H */

#include <dwavdapi/avdesk.h>
#include <dwavdapi/server.h>
#include <dwavdapi/admins.h>
#include <dwavdapi/groups.h>
#include <dwavdapi/tariffs.h>
#include <dwavdapi/stations.h>

#include "php_utils.h"
#include "php_dwavd.h"

static int le_dwavd = -1;
static int le_dwavd_srv = -1;
static int le_dwavd_srv_key = -1;
static int le_dwavd_adm = -1;
static int le_dwavd_grp = -1;
static int le_dwavd_grp_stats = -1;
static int le_dwavd_trf = -1;
static int le_dwavd_components_list = -1;
static int le_dwavd_component = -1;
static int le_dwavd_components_running_list = -1;
static int le_dwavd_component_running = -1;
static int le_dwavd_components_installed_list = -1;
static int le_dwavd_component_installed = -1;
static int le_dwavd_rights_list = -1;
static int le_dwavd_right = -1;
static int le_dwavd_bases_list = -1;
static int le_dwavd_base = -1;
static int le_dwavd_packages_list = -1;
static int le_dwavd_package = -1;
static int le_dwavd_modules_list = -1;
static int le_dwavd_module = -1;
static int le_dwavd_st = -1;
static int le_dwavd_st_stats = -11;
static int le_dwavd_trf_list = -1;
static int le_dwavd_grp_list = -1;
static int le_dwavd_adm_list = -1;
static int le_dwavd_srv_stats = -1;
static int le_dwavd_stats_scans = -1;
static int le_dwavd_stats_infections = -1;
static int le_dwavd_stats_traffic = -1;
static int le_dwavd_stations_state = -1;
static int le_dwavd_viruses_list = -1;
static int le_dwavd_virus = -1;
static int le_dwavd_infcd_objs_list = -1;
static int le_dwavd_infcd_obj = -1;

static int _dwavd_lst_res[12];
static unsigned int _dwavd_lst_res_count = 0;

ZEND_DECLARE_MODULE_GLOBALS(dwavd)

int _dwavd_opt_to_flag(const char *opt, const _dwavd_opt *arr, unsigned int size_arr);
static void _dwavd_right_array(zval **php_array, const dwavdapi_right *rht);
static void _dwavd_component_array(zval **php_array, const dwavdapi_component *cmp);
static void _dwavd_res_list_to_array(zval **php_array TSRMLS_DC, dwavdapi_list *lst, int res_found_type);
static void _dwavd_component_running_array(zval **php_array, const dwavdapi_running_component *cmp);
static void _dwavd_component_installed_array(zval **php_array, const dwavdapi_installed_component *cmp);
static void _dwavd_base_array(zval **php_array, const dwavdapi_base *base);
static void _dwavd_package_array(zval **php_array, const dwavdapi_package *package);
static void _dwavd_module_array(zval **php_array, const dwavdapi_module *module);
static void _dwavd_scans_array(zval **php_array, const dwavdapi_statistics_scans *scans);
static void _dwavd_infcd_array(zval **php_array, const dwavdapi_statistics_infections *infcd);
static void _dwavd_stats_array(zval **php_array, const dwavdapi_statistics_stations_state *state);
static void _dwavd_traffic_array(zval **php_array, const dwavdapi_statistics_traffic *traffic);
static void _dwavd_virus_array(zval **php_array, const dwavdapi_virus *virus);
static int _dwavd_adm_set(dwavdapi_admin *adm, int flag TSRMLS_DC, zval *val);
static int _dwavd_grp_set(dwavdapi_group *grp, int flag TSRMLS_DC, zval *val);
static int _dwavd_trf_set(dwavdapi_group *trf, int flag TSRMLS_DC, zval *val);
static int _dwavd_st_set(dwavdapi_station *st, int flag TSRMLS_DC, zval *val);


/** ======= Server info resource ======= */

/** Resource parameters available
 
    api_build - Dr.Web AV-Desk XML API version build number
    api_version - Dr.Web AV-Desk XML API version number
    groups_custom - Custom user groups count
    groups_system - System groups count
    groups_total - Groups total count
    tariffs_total - Tariff groups total count
    stations_total - Stations total count
    stations_licensed - 
    stations_available - Stations limit
    host - 
    os - 
    platform - 
    id - Dr.Web AV-Desk server ID
    version - Dr.Web AV-Desk version number
    uptime - Uptime in seconds
*/
static _dwavd_opt _dwavd_srvopt_array[] = {
    {"api_build", DWAVD_SRV_API_BUILD},
    {"api_version", DWAVD_SRV_API_VERSION},
    {"groups_custom", DWAVD_SRV_GROUPS_CUSTOM},
    {"groups_system", DWAVD_SRV_GROUPS_SYSTEM},
    {"groups_total", DWAVD_SRV_GROUPS_TOTAL},
    {"tariffs_total", DWAVD_SRV_TARIFFS_TOTAL},
    {"stations_total", DWAVD_SRV_STATIONS_TOTAL},
    {"stations_licensed", DWAVD_SRV_STATIONS_LICENSED},
    {"stations_available", DWAVD_SRV_STATIONS_AVAILABLE},
    {"host", DWAVD_SRV_HOST},
    {"os", DWAVD_SRV_OS},
    {"platform", DWAVD_SRV_PLATFORM},
    {"id", DWAVD_SRV_ID},
    {"version", DWAVD_SRV_VERSION},
    {"uptime", DWAVD_SRV_UPTIME}
};


/** ======= Server key resource ======= */

/** Resource parameters available

    activated - Activated at timestamp
    created - Created at timestamp
    expires - Expires at timestamp
    antispam - Antispam availability flag
    clients - Clients limit
    dealer - Dealer ID
    dealer_name - Dealer name
    user - Key owner ID
    user_name - Key owner name
    id - Key ID
    md5 - Key hash
    products - Products limit
    servers - Servers limit
    sn - key serial number
*/
static _dwavd_opt _dwavd_srvkeyopt_array[] = {
    {"activated", DWAVD_SRVKEY_ACTIVATED},
    {"created", DWAVD_SRVKEY_CREATED},
    {"expires", DWAVD_SRVKEY_EXPIRES},
    {"antispam", DWAVD_SRVKEY_ANTISPAM},
    {"clients", DWAVD_SRVKEY_CLIENTS},
    {"dealer", DWAVD_SRVKEY_DEALER},
    {"dealer_name", DWAVD_SRVKEY_DEALER_NAME},
    {"user", DWAVD_SRVKEY_USER},
    {"user_name", DWAVD_SRVKEY_USER_NAME},
    {"id", DWAVD_SRVKEY_ID},
    {"md5", DWAVD_SRVKEY_MD5},
    {"products", DWAVD_SRVKEY_PRODUCTS},
    {"servers", DWAVD_SRVKEY_SERVERS},
    {"sn", DWAVD_SRVKEY_SN}
};


/** ======= Administrator info resource ======= */

/** Resource parameters available

    created - Created at timestamp
    description - 
    groups - List of administrated groups IDs
    groups_count - 
    id - Administrator unique ID
    last_name - 
    login - 
    middle_name - 
    modified - Modified at timestamp
    name - First name
    password - 
    type - Type ID: DWAVD_ADMIN_TYPE_GLOBAL_ADMIN or DWAVD_ADMIN_TYPE_GROUP_ADMIN
    readonly - Readonly boolean flag
    limited_rights - Limited rights boolean flag
    delete_from_groups - Groups IDs list to remove the administrator from
*/
static _dwavd_opt _dwavd_admopt_array[] = {
    {"created", DWAVD_ADM_CREATED},
    {"description", DWAVD_ADM_DESCRIPTION},
    {"groups", DWAVD_ADM_GROUPS},
    {"groups_count", DWAVD_ADM_GROUPS_COUNT},
    {"id", DWAVD_ADM_ID},
    {"last_name", DWAVD_ADM_LAST_NAME},
    {"login", DWAVD_ADM_LOGIN},
    {"middle_name", DWAVD_ADM_MIDDLE_NAME},
    {"modified", DWAVD_ADM_MODIFIED},
    {"name", DWAVD_ADM_NAME},
    {"password", DWAVD_ADM_PASSWORD},
    {"type", DWAVD_ADM_TYPE},
    {"readonly", DWAVD_ADM_READ_ONLY},
    {"limited_rights", DWAVD_ADM_LIMITED_RIGHTS},
    {"delete_from_groups", DWAVD_ADM_DEL_FROM_GROUPS}
};


/** ======= Group info resource ======= */

/** Resource parameters available

    created - Created at timestamp
    description - 
    child_groups - List of child groups IDs
    child_groups_count - 
    id - Group unique ID
    modified - Modified at timestamp
    name - Title
    type - Group type ID
    admins - List of administrators IDs for the group
    admins_count - 
    emails - List of e-mails for the group 
    emails_count - 
    stations -  List of stations IDs in the group
    stations_count - 
    rights - Agent rights preset for the group
    parent_id - Parent group ID
    delete_emails - E-mails list to remove from group resource
 */
static _dwavd_opt _dwavd_grpopt_array[] = {
    {"created", DWAVD_GRP_CREATED},
    {"description", DWAVD_GRP_DESCRIPTION},
    {"child_groups", DWAVD_GRP_CHILD_GROUPS},
    {"child_groups_count", DWAVD_GRP_CHILD_GROUPS_COUNT},
    {"id", DWAVD_GRP_ID},
    {"modified", DWAVD_GRP_MODIFIED},
    {"name", DWAVD_GRP_NAME},    
    {"type", DWAVD_GRP_TYPE},
    {"admins", DWAVD_GRP_ADMINS},
    {"admins_count", DWAVD_GRP_ADMINS_COUNT},
    {"emails", DWAVD_GRP_EMAILS},
    {"emails_count", DWAVD_GRP_EMAILS_COUNT},
    {"stations", DWAVD_GRP_STATIONS},
    {"stations_count", DWAVD_GRP_STATIONS_COUNT},
    {"rights", DWAVD_GRP_RIGHTS},
    {"parent_id", DWAVD_GRP_PARENT_ID},
    {"delete_emails", DWAVD_GRP_DEL_EMAILS}
};


/** ======= Tariff group info resource ======= */

/** Resource parameters available

    created - Created at timestamp
    description - 
    child_groups - List of child groups IDs
    child_groups_count - 
    id - Tariff group unique ID
    modified - Modified at timestamp
    name - Title
    type - Tariff group type ID
    stations - List of stations using the tariff
    stations_count - 
    grace_period - 
    components - AV components preset for the tariff
    rights - Agent rights preset for the tariff
    parent_id - Parent group ID
 
 */ 
static _dwavd_opt _dwavd_trfopt_array[] = {
    {"created", DWAVD_TRF_CREATED},
    {"description", DWAVD_TRF_DESCRIPTION},
    {"child_groups", DWAVD_TRF_CHILD_GROUPS},
    {"child_groups_count", DWAVD_TRF_CHILD_GROUPS_COUNT},
    {"id", DWAVD_TRF_ID},
    {"modified", DWAVD_TRF_MODIFIED},
    {"name", DWAVD_TRF_NAME},    
    {"type", DWAVD_TRF_TYPE},
    {"stations", DWAVD_TRF_STATIONS},
    {"stations_count", DWAVD_TRF_STATIONS_COUNT},
    {"grace_period", DWAVD_TRF_GRACE_PERIOD},
    {"components", DWAVD_TRF_COMPONENTS},
    {"rights", DWAVD_TRF_RIGHTS},
    {"parent_id", DWAVD_TRF_PARENT_ID},
};


/** ======= AV Component resource ======= */

/** Component resource parameters available

    code - 
    name - Title
    value - State/value
    inherited_group_id - Inherited from group ID
*/
static _dwavd_opt _dwavd_cmpopt_array[] = {
    {"code", DWAVD_CMP_CODE},
    {"name", DWAVD_CMP_NAME},
    {"value", DWAVD_CMP_VALUE},
    {"inherited_group_id", DWAVD_CMP_INHERITED_GROUP_ID}
};


/** Installed component resource parameters available

    name - Title
    installed - Installed at timestamp
    server - Installed from server
    path - Installation path
*/
static _dwavd_opt _dwavd_cmp_instd_opt_array[] = {
    {"name", DWAVD_CMP_INSTD_NAME},
    {"installed", DWAVD_CMP_INSTD_INSTALLED},
    {"server", DWAVD_CMP_INSTD_SERVER},
    {"path", DWAVD_CMP_INSTD_PATH}
};


/** Running component resource parameters available

    name - Title
    started - Started at timestamp
    type - Type ID
    user - Run by user name
    params - Startup parameters
*/
static _dwavd_opt _dwavd_cmp_run_opt_array[] = {
    {"name", DWAVD_CMP_RUN_NAME},
    {"started", DWAVD_CMP_RUN_STARTED},
    {"type", DWAVD_CMP_RUN_TYPE},
    {"user", DWAVD_CMP_RUN_USER},
    {"params", DWAVD_CMP_RUN_PARAMS}
};


/** ======= Station rights resource ======= */

/** Resource parameters available

    code - Right ID
    name - Title
    value - State/value
    inherited_group_id - Inherited from group ID
*/
static _dwavd_opt _dwavd_rhtopt_array[] = {
    {"code", DWAVD_RHT_CODE},
    {"name", DWAVD_RHT_NAME},
    {"value", DWAVD_RHT_VALUE},
    {"inherited_group_id", DWAVD_RHT_INHERITED_GROUP_ID}
};


/** ======= AV Base resource ======= */

/** Resource parameters available

    file - Filename
    version - 
    viruses - Viruses count
    created - Created at timestamp
*/
static _dwavd_opt _dwavd_baseopt_array[] = {
    {"file", DWAVD_BASE_FILE},
    {"version", DWAVD_BASE_VERSION},
    {"viruses", DWAVD_BASE_VIRUSES},
    {"created", DWAVD_BASE_CREATED}
};


/** ======= AV-Agent Package resource ======= */

/** Resource parameters available

    url - Package download URL
    type - Package platform type
*/
static _dwavd_opt _dwavd_packopt_array[] = {
    {"url", DWAVD_PACKAGE_URL},
    {"type", DWAVD_PACKAGE_TYPE}
};

/** ======= AV Module resource ======= */

/** Resource parameters available

    file - Filename
    version - 
    created - Created at timestamp
    modified - Modified at timestamp
    hash - Module hash
    size - 
    name - Title
*/
static _dwavd_opt _dwavd_moduleopt_array[] = {
    {"file", DWAVD_MOD_FILE},
    {"version", DWAVD_MOD_VERSION},
    {"created", DWAVD_MOD_CREATED},
    {"modified", DWAVD_MOD_MODIFIED},
    {"hash", DWAVD_MOD_HASH},
    {"size", DWAVD_MOD_SIZE},
    {"name", DWAVD_MOD_NAME}
};

/** ======= Station info resource ======= */

/** Resource parameters available
 
    os - Operating system ID
    os_name - Operating system name
    state - Station state
    country - Country ID
    country_name - Country name
    country_code - Country ISO code
    grace_period - 
    longitude - 
    latitude - 
    created - Created at timestamp
    modified - Modified at timestamp
    expires - Expires at timestamp
    city - 
    floor - 
    url - Agent download URL
    config - Agent configuration download URL
    password - Agent password
    description - 
    id - Station unique ID
    room - 
    name - Station name (automatically deduced from computer name)
    street - 
    province - 
    department - 
    tariff_id - Tariff ID for station
    organization - 
    parent_id - Station main parent group ID
    emails - List of e-mails for the station 
    emails_count - 
    groups - Station additional pernt groups IDs
    groups_count - 
    components - AV components
    bases - AV bases
    packages - AV-agent packages
    modules - AV modules
    rights - Restrictions for agent
    components_running - AV components currently running at station 
    components_installed - AV components installed at station 
    block_begins - Block period begin timestamp
    block_ends - Block period end timestamp
    lastseen_time - 
    lastseen_addr - Network address
    delete_emails - E-mails list to remove from station resource
 */
static _dwavd_opt _dwavd_stopt_array[] = {
    {"os", DWAVD_ST_OS},
    {"os_name", DWAVD_ST_OS_NAME},
    {"state", DWAVD_ST_STATE},
    {"country", DWAVD_ST_COUNTRY},
    {"country_name", DWAVD_ST_COUNTRY_NAME},
    {"country_code", DWAVD_ST_COUNTRY_CODE},
    {"grace_period", DWAVD_ST_GRACE_PERIOD},
    {"longitude", DWAVD_ST_LONGITUDE},
    {"latitude", DWAVD_ST_LATITUDE},
    {"created", DWAVD_ST_CREATED},
    {"modified", DWAVD_ST_MODIFIED},
    {"expires", DWAVD_ST_EXPIRES},
    {"city", DWAVD_ST_CITY},
    {"floor", DWAVD_ST_FLOOR},
    {"url", DWAVD_ST_URL},
    {"config", DWAVD_ST_CONFIG},
    {"password", DWAVD_ST_PASSWORD},
    {"description", DWAVD_ST_DESCRIPTION},
    {"id", DWAVD_ST_ID},
    {"room", DWAVD_ST_ROOM},
    {"name", DWAVD_ST_NAME},
    {"street", DWAVD_ST_STREET},
    {"province", DWAVD_ST_PROVINCE},
    {"department", DWAVD_ST_DEPARTMENT},
    {"tariff_id", DWAVD_ST_TARIFF_ID},
    {"organization", DWAVD_ST_ORGANIZATION},
    {"parent_id", DWAVD_ST_PARENT_ID},
    {"emails", DWAVD_ST_EMAILS},
    {"emails_count", DWAVD_ST_EMAILS_COUNT},
    {"groups", DWAVD_ST_GROUPS},
    {"groups_count", DWAVD_ST_GROUPS_COUNT},
    {"components", DWAVD_ST_COMPONENTS},
    {"bases", DWAVD_ST_BASES},
    {"packages", DWAVD_ST_PACKAGES},
    {"modules", DWAVD_ST_MODULES},
    {"rights", DWAVD_ST_RIGHTS},
    {"components_running", DWAVD_ST_COMPONENTS_RUN},
    {"components_installed", DWAVD_ST_COMPONENTS_INSTD},
    {"block_begins", DWAVD_ST_BLOCK_BEGINS},
    {"block_ends", DWAVD_ST_BLOCK_ENDS},
    {"lastseen_time", DWAVD_ST_LASTSEEN_TIME},
    {"lastseen_addr", DWAVD_ST_LASTSEEN_ADDR},
    {"delete_emails", DWAVD_ST_DEL_EMAILS},
    {"scc", DWAVD_ST_SCC}
};


/** ======= Stats: Server resource ======= */

/** Resource parameters available

    tariffs_total - 
    groups_total - 
    groups_custom - 
    groups_system - 
    stations_total - 
    stations_state - 
    traffic - 
    scans - 
    infections - 
    viruses -  
*/
static _dwavd_opt _dwavd_srv_stats_opt_array[] = {
    {"tariffs_total", DWAVD_SRV_STATS_TARIFFS_TOTAL},
    {"groups_total", DWAVD_SRV_STATS_GROUPS_TOTAL},
    {"groups_custom", DWAVD_SRV_STATS_GROUPS_CUSTOM},
    {"groups_system", DWAVD_SRV_STATS_GROUPS_SYSTEM},
    {"stations_total", DWAVD_SRV_STATS_STATIONS_TOTAL},
    {"stations_state", DWAVD_SRV_STATS_STATIONS_STATE},
    {"traffic", DWAVD_SRV_STATS_TRAFFIC},
    {"scans", DWAVD_SRV_STATS_SCANS},
    {"infections", DWAVD_SRV_STATS_INFECTIONS},
    {"viruses", DWAVD_SRV_STATS_VIRUSES}    
};


/** ======= Stations states helpers ======= */

/** Resource parameters available

    online - 
    deinstalled - 
    blocked - 
    expired - 
    offline - 
    activated - 
    unactivated - 
    total -  
*/
static _dwavd_opt _dwavd_st_state_opt_array[] = { 
    {"online", DWAVD_ST_STATE_ONLINE},
    {"deinstalled", DWAVD_ST_STATE_DEINSTALLED},
    {"blocked", DWAVD_ST_STATE_BLOCKED},
    {"expired", DWAVD_ST_STATE_EXPIRED},
    {"offline", DWAVD_ST_STATE_OFFLINE},
    {"activated", DWAVD_ST_STATE_ACTIVATED},
    {"unactivated", DWAVD_ST_STATE_UNACTIVATED},
    {"total", DWAVD_ST_STATE_TOTAL}
};

/** ======= Stats: Scan helpers ======= */

/** Resource parameters available

    size - 
    files - 
    deleted - 
    moved - 
    cured - 
    errors - 
    infected - 
    renamed - 
    locked -  
*/
static _dwavd_opt _dwavd_scans_opt_array[] = {
    {"size", DWAVD_SCANS_SIZE},
    {"files", DWAVD_SCANS_FILES},
    {"deleted", DWAVD_SCANS_DELETED},
    {"moved", DWAVD_SCANS_MOVED},
    {"cured", DWAVD_SCANS_CURED},
    {"errors", DWAVD_SCANS_ERRORS},
    {"infected", DWAVD_SCANS_INFECTED},
    {"renamed", DWAVD_SCANS_RENAMED},
    {"locked", DWAVD_SCANS_LOCKED}   
};        


/** ======= Stats: Infections helpers ======= */

/** Resource parameters available

    deleted - 
    cured - 
    moved - 
    incurable - 
    locked - 
    renamed - 
    errors - 
    ignored - 
    total -  
*/
static _dwavd_opt _dwavd_infcd_opt_array[] = {
    {"deleted", DWAVD_INFCD_DELETED},
    {"cured", DWAVD_INFCD_CURED},
    {"moved", DWAVD_INFCD_MOVED},
    {"incurable", DWAVD_INFCD_INCURABLE},
    {"locked", DWAVD_INFCD_LOCKED},
    {"renamed", DWAVD_INFCD_RENAMED},
    {"errors", DWAVD_INFCD_ERRORS},
    {"ignored", DWAVD_INFCD_IGNORED},
    {"total", DWAVD_INFCD_TOTAL}
};


/** ======= Stats: Traffic helpers ======= */

/** Resource parameters available

    total - 
    in - 
    out -   
*/
static _dwavd_opt _dwavd_traffic_opt_array[] = {
    {"total", DWAVD_TRAFFIC_TOTAL},
    {"in", DWAVD_TRAFFIC_IN},
    {"out", DWAVD_TRAFFIC_OUT}
};


/** ======= Virus resource ======= */

/** Resource parameters available

    name - 
    objects - 
    objects_count -   
*/
static _dwavd_opt _dwavd_virus_opt_array[] = {
    {"name", DWAVD_VIRUS_NAME},
    {"objects", DWAVD_VIRUS_OBJECTS},
    {"objects_count", DWAVD_VIRUS_OBJECTS_COUNT}
};


/** ======= Infected object resource ======= */

/** Resource parameters available

    type - 
    cure - 
    type_code - 
    cure_code - 
    infection_type - 
    infection_type_code - 
    originator - 
    originator_code - 
    path - 
    owner - 
    station_id - 
    username - 
*/
static _dwavd_opt _dwavd_infcd_objs_opt_array[] = {
    {"type", DWAVD_INFCD_OBJ_TYPE},
    {"cure", DWAVD_INFCD_OBJ_CURE},
    {"type_code", DWAVD_INFCD_OBJ_TYPE_CODE},
    {"cure_code", DWAVD_INFCD_OBJ_CURE_CODE},
    {"infection_type", DWAVD_INFCD_OBJ_INFECTION_TYPE},
    {"infection_type_code", DWAVD_INFCD_OBJ_INFECTION_TYPE_CODE},
    {"originator", DWAVD_INFCD_OBJ_ORIGINATOR},
    {"originator_code", DWAVD_INFCD_OBJ_ORIGINATOR_CODE},
    {"path", DWAVD_INFCD_OBJ_PATH},
    {"owner", DWAVD_INFCD_OBJ_OWNER},
    {"station_id", DWAVD_INFCD_OBJ_STATION_ID},
    {"username", DWAVD_INFCD_OBJ_USERNAME}
};


/** ======= Stats: Group resource ======= */

/** Resource parameters available

    stations_total - 
    stations_state - 
    scans - 
    infections - 
    viruses -   
*/
static _dwavd_opt _dwavd_grp_stats_opt_array[] = {
    {"stations_total", DWAVD_GRP_STATS_STATIONS_TOTAL},
    {"stations_state", DWAVD_GRP_STATS_STATIONS_STATE},
    {"scans", DWAVD_GRP_STATS_SCANS},
    {"infections", DWAVD_GRP_STATS_INFECTIONS},
    {"viruses", DWAVD_GRP_STATS_VIRUSES}
};


/** ======= Stats: Station resource ======= */

/** Resource parameters available

    scans - 
    infections - 
    viruses -   
*/
static _dwavd_opt _dwavd_st_stats_opt_array[] = {
    {"scans", DWAVD_ST_STATS_SCANS},
    {"infections", DWAVD_ST_STATS_INFECTIONS},
    {"viruses", DWAVD_ST_STATS_VIRUSES}
};

PHP_INI_DISP(php_dwavd_display_password) {
    if (sapi_module.phpinfo_as_text) {
        php_printf("******");
    } else {
        php_printf("<i>hidden</i>");
    }
}

int _dwavd_opt_to_flag(const char *opt, const _dwavd_opt *arr, unsigned int size_arr) {
    unsigned int i = 0;
    for (i = 0; i < size_arr; i++) {
        if (strcasecmp(opt, (arr+i)->opt) == 0) {
            return (arr+i)->falg;
        }
    }
    return -1;
}

static void _dwavd_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_handle *handle = (dwavdapi_handle *) rsrc->ptr;
    dwavdapi_destroy(handle);
}

static void _dwavd_srv_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_server *srv = (dwavdapi_server *) rsrc->ptr;
    dwavdapi_srv_info_destroy(srv);
}

static void _dwavd_srv_key_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_server_key *key = (dwavdapi_server_key *) rsrc->ptr;
    dwavdapi_srv_key_info_destroy(key);
}

static void _dwavd_srv_stats_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_server_statistics *stats = (dwavdapi_server_statistics *) rsrc->ptr;
    dwavdapi_srv_statistics_destroy(stats);
}

static void _dwavd_adm_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_admin *adm = (dwavdapi_admin *) rsrc->ptr;
    dwavdapi_admin_destroy(adm);
}

static void _dwavd_grp_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_group *grp = (dwavdapi_group *) rsrc->ptr;
    dwavdapi_group_destroy(grp);
}

static void _dwavd_grp_stats_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_group_statistics *stats = (dwavdapi_group_statistics *) rsrc->ptr;
    dwavdapi_group_statistics_destroy(stats);
}

static void _dwavd_st_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_station *st = (dwavdapi_station *) rsrc->ptr;
    dwavdapi_station_destroy(st);
}

static void _dwavd_components_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_list *lst = (dwavdapi_list *) rsrc->ptr;
    dwavdapi_list_destroy(lst, (dwavdapi_dtor_funct)dwavdapi_component_destroy);
}

static void _dwavd_components_running_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_list *lst = (dwavdapi_list *) rsrc->ptr;
    dwavdapi_list_destroy(lst, (dwavdapi_dtor_funct)dwavdapi_component_running_destroy);
}

static void _dwavd_components_installed_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_list *lst = (dwavdapi_list *) rsrc->ptr;
    dwavdapi_list_destroy(lst, (dwavdapi_dtor_funct)dwavdapi_component_installed_destroy);
}

static void _dwavd_modules_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_list *lst = (dwavdapi_list *) rsrc->ptr;
    dwavdapi_list_destroy(lst, (dwavdapi_dtor_funct)dwavdapi_module_destroy);
}

static void _dwavd_bases_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_list *lst = (dwavdapi_list *) rsrc->ptr;
    dwavdapi_list_destroy(lst, (dwavdapi_dtor_funct)dwavdapi_base_destroy);
}

static void _dwavd_packages_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_list *lst = (dwavdapi_list *) rsrc->ptr;
    dwavdapi_list_destroy(lst, (dwavdapi_dtor_funct)dwavdapi_package_destroy);
}

static void _dwavd_rights_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_list *lst = (dwavdapi_list *) rsrc->ptr;
    dwavdapi_list_destroy(lst, (dwavdapi_dtor_funct)dwavdapi_right_destroy);
}

static void _dwavd_grp_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_list *lst = (dwavdapi_list *) rsrc->ptr;
    dwavdapi_list_destroy(lst, (dwavdapi_dtor_funct)dwavdapi_group_destroy);
}

static void _dwavd_adm_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_list *lst = (dwavdapi_list *) rsrc->ptr;
    dwavdapi_list_destroy(lst, (dwavdapi_dtor_funct)dwavdapi_admin_destroy);
}

static void _dwavd_viruses_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_list *lst = (dwavdapi_list *) rsrc->ptr;
    dwavdapi_list_destroy(lst, (dwavdapi_dtor_funct)dwavdapi_virus_destroy);
}

static void _dwavd_infcd_objs_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_list *lst = (dwavdapi_list *) rsrc->ptr;
    dwavdapi_list_destroy(lst, (dwavdapi_dtor_funct)dwavdapi_infected_object_destroy);
}

static void _dwavd_st_stats_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
    dwavdapi_station_statistics *stats = (dwavdapi_station_statistics *) rsrc->ptr;
    dwavdapi_station_statistics_destroy(stats);
}

static void _dwavd_stats_array(zval **php_array, const dwavdapi_statistics_stations_state *state) {
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != state) {  
        add_assoc_long(array, "activated", dwavdapi_statistics_stations_state_activated(state));
        add_assoc_long(array, "unactivated", dwavdapi_statistics_stations_state_unactivated(state));
        add_assoc_long(array, "blocked", dwavdapi_statistics_stations_state_blocked(state));
        add_assoc_long(array, "deinstalled", dwavdapi_statistics_stations_state_deinstalled(state));
        add_assoc_long(array, "expired", dwavdapi_statistics_stations_state_expired(state));
        add_assoc_long(array, "offline", dwavdapi_statistics_stations_state_offline(state));
        add_assoc_long(array, "online", dwavdapi_statistics_stations_state_online(state));
        add_assoc_long(array, "total", dwavdapi_statistics_stations_state_total(state));
    }
    *php_array = array;
}

static void _dwavd_traffic_array(zval **php_array, const dwavdapi_statistics_traffic *traffic) {
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != traffic) {  
        add_assoc_double(array, "in", dwavdapi_srv_traffic_in(traffic));
        add_assoc_double(array, "out", dwavdapi_srv_traffic_out(traffic));
        add_assoc_double(array, "total", dwavdapi_srv_traffic_total(traffic));
    }
    *php_array = array;
}

static void _dwavd_virus_array(zval **php_array, const dwavdapi_virus *virus) {
    zval *array = NULL;
    zval *objects_array = NULL;
    dwavdapi_list *list = NULL;
    
    TSRMLS_FETCH();
    
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != virus) {  
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "name", dwavdapi_virus_name(virus));
        add_assoc_long(array, "objects_count", dwavdapi_virus_infected_objects_count(virus));        
        list = dwavdapi_virus_infected_objects_list(virus);
        _dwavd_res_list_to_array(&objects_array TSRMLS_CC, list, le_dwavd_infcd_objs_list);
        add_assoc_zval(array, "objects", objects_array);
        
    }
    *php_array = array;
}

static void _dwavd_infcd_array(zval **php_array, const dwavdapi_statistics_infections *infcd) {
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != infcd) {  
        add_assoc_long(array, "cured", dwavdapi_statistics_infections_cured(infcd));
        add_assoc_long(array, "deleted", dwavdapi_statistics_infections_deleted(infcd));
        add_assoc_long(array, "errors", dwavdapi_statistics_infections_errors(infcd));
        add_assoc_long(array, "ignored", dwavdapi_statistics_infections_ignored(infcd));
        add_assoc_long(array, "incurable", dwavdapi_statistics_infections_incurable(infcd));
        add_assoc_long(array, "locked", dwavdapi_statistics_infections_locked(infcd));
        add_assoc_long(array, "moved", dwavdapi_statistics_infections_moved(infcd));
        add_assoc_long(array, "renamed", dwavdapi_statistics_infections_renamed(infcd));
        add_assoc_long(array, "total", dwavdapi_statistics_infections_total(infcd));
    }
    *php_array = array;
}

static void _dwavd_right_array(zval **php_array, const dwavdapi_right *rht) {
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != rht) {
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "name", (char *)dwavdapi_right_name(rht)) 
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "inherited_group_id", (char *)dwavdapi_right_inherited_group_id(rht)) 
        add_assoc_long(array, "code", dwavdapi_right_code(rht)); 
        add_assoc_long(array, "value", dwavdapi_right_value(rht)); 
    }
    *php_array = array;
}

static void _dwavd_component_installed_array(zval **php_array, const dwavdapi_installed_component *cmp) {
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != cmp) {
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "name", (char *)dwavdapi_component_installed_name(cmp)) 
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "path", (char *)dwavdapi_component_installed_path(cmp))
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "server", (char *)dwavdapi_component_installed_server(cmp)) 
        add_assoc_long(array, "installed", dwavdapi_component_installed_time(cmp)); 
    }
    *php_array = array;
}
static void _dwavd_component_array(zval **php_array, const dwavdapi_component *cmp) {
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != cmp) {
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "name", (char *)dwavdapi_component_name(cmp)) 
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "inherited_group_id", (char *)dwavdapi_component_inherited_group_id(cmp)) 
        add_assoc_long(array, "code", dwavdapi_component_code(cmp)); 
        add_assoc_long(array, "value", dwavdapi_component_value(cmp));
    }
    *php_array = array;
}

static void _dwavd_component_running_array(zval **php_array, const dwavdapi_running_component *cmp) {
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != cmp) {
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "name", (char *)dwavdapi_component_running_name(cmp)) 
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "params", (char *)dwavdapi_component_running_params(cmp))
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "user", (char *)dwavdapi_component_running_user(cmp)) 
        add_assoc_long(array, "started", dwavdapi_component_running_started_time(cmp)); 
        add_assoc_long(array, "type", dwavdapi_component_running_type(cmp));
    }
    
    *php_array = array;
}


static void _dwavd_scans_array(zval **php_array, const dwavdapi_statistics_scans *scans) {
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != scans) { 
        add_assoc_double(array, "size", dwavdapi_statistics_scans_size(scans)); 
        add_assoc_double(array, "files", dwavdapi_statistics_scans_files(scans)); 
        add_assoc_long(array, "deleted", dwavdapi_statistics_scans_deleted(scans)); 
        add_assoc_long(array, "moved", dwavdapi_statistics_scans_moved(scans));
        add_assoc_long(array, "cured", dwavdapi_statistics_scans_cured(scans));
        add_assoc_long(array, "errors", dwavdapi_statistics_scans_errors(scans));
        add_assoc_long(array, "infected", dwavdapi_statistics_scans_infected(scans));
        add_assoc_long(array, "renamed", dwavdapi_statistics_scans_renamed(scans));
        add_assoc_long(array, "locked", dwavdapi_statistics_scans_locked(scans));
    }
    *php_array = array;
}

static void _dwavd_base_array(zval **php_array, const dwavdapi_base *base) {
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != base) {
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "file", (char *)dwavdapi_base_file_name(base)) 
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "version", (char *)dwavdapi_base_version(base))
        add_assoc_long(array, "viruses", dwavdapi_base_viruses(base));
        add_assoc_long(array, "created", dwavdapi_base_created_time(base)); 
    }
    *php_array = array;
}

static void _dwavd_package_array(zval **php_array, const dwavdapi_package *package) {
    zval *array = NULL;
    
    MAKE_STD_ZVAL(array)
    array_init(array);
    if (package!=NULL) {
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "url", (char *)dwavdapi_package_url(package)) 
        add_assoc_long(array, "type", dwavdapi_package_type(package)); 
    }
    *php_array = array;
}

static void _dwavd_infcd_obj_array(zval **php_array, const dwavdapi_infected_object *obj) {
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != obj) { 
        add_assoc_long(array, "cure_code", dwavdapi_infected_object_cure_status(obj)); 
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "cure", (char *)dwavdapi_infected_object_cure_status_str(obj)); 
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "owner", dwavdapi_infected_object_owner(obj)); 
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "path", dwavdapi_infected_object_path(obj)); 
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "type", (char *)dwavdapi_infected_object_type_str(obj));
        add_assoc_long(array, "type_code", dwavdapi_infected_object_type(obj)); 
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "type", dwavdapi_infected_object_username(obj));
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "infection_type", (char *)dwavdapi_infection_type_str(obj));
        add_assoc_long(array, "infection_type_code", dwavdapi_infection_type(obj));
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "station_id", dwavdapi_infection_station_id(obj));
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "originator", (char *)dwavdapi_application_name(dwavdapi_infection_originator(obj)));
        add_assoc_long(array, "originator_code", dwavdapi_infection_originator(obj));
    }
    *php_array = array;
}

static void _dwavd_globals_init(zend_dwavd_globals *dwavd_globals) {
    dwavd_globals->avdesk_port = 0;
    dwavd_globals->avdesk_host = NULL;
    dwavd_globals->avdesk_login = NULL;
    dwavd_globals->avdesk_password = NULL;
    dwavd_globals->avdesk_crt = NULL;
}


/** ======= Basics ======= */

/** php.ini parameters

    avdesk.host - AV-Desk webserver URL/host
    avdesk.port - AV-Desk web-server port (default: 9080)
    avdesk.login - Administrator login
    avdesk.password - Administrator password
    avdesk.crt - Path to SSL certificate
  
*/
PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("avdesk.host", "localhost", PHP_INI_ALL, OnUpdateString, avdesk_host, zend_dwavd_globals, dwavd_globals)
STD_PHP_INI_ENTRY("avdesk.port", "9080", PHP_INI_ALL, OnUpdateLong, avdesk_port, zend_dwavd_globals, dwavd_globals)
STD_PHP_INI_ENTRY("avdesk.login", "admin", PHP_INI_ALL, OnUpdateString, avdesk_login, zend_dwavd_globals, dwavd_globals)
STD_PHP_INI_ENTRY("avdesk.crt", "", PHP_INI_ALL, OnUpdateString, avdesk_crt, zend_dwavd_globals, dwavd_globals)
STD_PHP_INI_ENTRY_EX("avdesk.password", "root", PHP_INI_ALL, OnUpdateString, avdesk_password, zend_dwavd_globals, dwavd_globals, php_dwavd_display_password)
PHP_INI_END()
           
ZEND_BEGIN_ARG_INFO_EX(ai_dwavd_error, 0, 0, 4)
ZEND_ARG_INFO(0, handle)
ZEND_ARG_INFO(1, errno)
ZEND_ARG_INFO(1, error)
ZEND_ARG_INFO(1, srv)
ZEND_END_ARG_INFO()

const zend_function_entry dwavd_functions[] = {
    PHP_FE(dwavd_init, NULL)
    PHP_FE(dwavd_free, NULL)
    PHP_FE(dwavd_switch_to_debug_mode, NULL)
    PHP_FE(dwavd_error, ai_dwavd_error)
    PHP_FE(dwavd_version, NULL)
    PHP_FE(dwavd_lib_version, NULL)
    PHP_FE(dwavd_set_host, NULL)
    PHP_FE(dwavd_set_port, NULL)
    PHP_FE(dwavd_set_user_agent, NULL)
    PHP_FE(dwavd_set_login, NULL)
    PHP_FE(dwavd_set_password, NULL)
    PHP_FE(dwavd_set_timeout, NULL)
    PHP_FE(dwavd_set_ssl_crt, NULL)
    PHP_FE(dwavd_srv_get_info, NULL)
    PHP_FE(dwavd_srv_free, NULL)
    PHP_FE(dwavd_srv_array, NULL)
    PHP_FE(dwavd_srv, NULL)
    PHP_FE(dwavd_srv_key_get_info, NULL)
    PHP_FE(dwavd_srv_key_free, NULL)
    PHP_FE(dwavd_srv_key_array, NULL)
    PHP_FE(dwavd_srv_key, NULL)
    PHP_FE(dwavd_srv_run_task, NULL)
    PHP_FE(dwavd_srv_get_stats, NULL)
    PHP_FE(dwavd_srv_stats_free, NULL)
    PHP_FE(dwavd_srv_stats, NULL)
    PHP_FE(dwavd_srv_stats_array, NULL)
    PHP_FE(dwavd_adm_get_info, NULL)
    PHP_FE(dwavd_adm_get_list, NULL)
    PHP_FE(dwavd_adm_free, NULL)
    PHP_FE(dwavd_adm_list_free, NULL)
    PHP_FE(dwavd_adm_array, NULL)
    PHP_FE(dwavd_adm, NULL)
    PHP_FE(dwavd_adm_add, NULL)
    PHP_FE(dwavd_adm_set, NULL)
    PHP_FE(dwavd_adm_set_array, NULL)
    PHP_FE(dwavd_adm_change, NULL)
    PHP_FE(dwavd_adm_delete, NULL)
    PHP_FE(dwavd_grp_get_info, NULL)
    PHP_FE(dwavd_grp_get_list, NULL)
    PHP_FE(dwavd_grp_free, NULL)
    PHP_FE(dwavd_grp_list_free, NULL)
    PHP_FE(dwavd_grp, NULL)
    PHP_FE(dwavd_grp_get_stats, NULL)
    PHP_FE(dwavd_grp_stats_free, NULL)
    PHP_FE(dwavd_grp_stats, NULL)
    PHP_FE(dwavd_grp_stats_array, NULL)
    PHP_FE(dwavd_grp_set, NULL)
    PHP_FE(dwavd_grp_set_array, NULL)
    PHP_FE(dwavd_grp_add, NULL)
    PHP_FE(dwavd_grp_change, NULL)
    PHP_FE(dwavd_grp_delete, NULL)
    PHP_FE(dwavd_grp_array, NULL)
    PHP_FE(dwavd_trf_get_info, NULL)
    PHP_FE(dwavd_trf_get_list, NULL)
    PHP_FE(dwavd_trf_free, NULL)
    PHP_FE(dwavd_trf, NULL)
    PHP_FE(dwavd_trf_set, NULL)
    PHP_FE(dwavd_trf_set_array, NULL)
    PHP_FE(dwavd_trf_set_component, NULL)
    PHP_FE(dwavd_trf_add, NULL)
    PHP_FE(dwavd_trf_change, NULL)
    PHP_FE(dwavd_trf_delete, NULL)
    PHP_FE(dwavd_trf_array, NULL)
    PHP_FE(dwavd_list_next, NULL)
    PHP_FE(dwavd_list_prev, NULL)
    PHP_FE(dwavd_list_rewind, NULL)
    PHP_FE(dwavd_list_current, NULL)
    PHP_FE(dwavd_component, NULL)
    PHP_FE(dwavd_component_array, NULL)
    PHP_FE(dwavd_right_array, NULL)
    PHP_FE(dwavd_right, NULL)
    PHP_FE(dwavd_list_array, NULL)
    PHP_FE(dwavd_st_get_info, NULL)
    PHP_FE(dwavd_st_free, NULL)
    PHP_FE(dwavd_st_array, NULL)
    PHP_FE(dwavd_st, NULL)
    PHP_FE(dwavd_st_get_stats, NULL)
    PHP_FE(dwavd_st_stats_free, NULL)
    PHP_FE(dwavd_st_stats, NULL)
    PHP_FE(dwavd_st_stats_array, NULL)
    PHP_FE(dwavd_st_set, NULL)
    PHP_FE(dwavd_st_set_array, NULL)
    PHP_FE(dwavd_st_delete, NULL)
    PHP_FE(dwavd_st_add, NULL)
    PHP_FE(dwavd_st_change, NULL)
    PHP_FE(dwavd_base, NULL)
    PHP_FE(dwavd_base_array, NULL)
    PHP_FE(dwavd_package, NULL)
    PHP_FE(dwavd_package_array, NULL)
    PHP_FE(dwavd_module, NULL)
    PHP_FE(dwavd_module_array, NULL)
    PHP_FE(dwavd_component_installed_array, NULL)
    PHP_FE(dwavd_component_installed, NULL)
    PHP_FE(dwavd_component_running_array, NULL)
    PHP_FE(dwavd_component_running, NULL)
    PHP_FE(dwavd_trf_list_free, NULL)
    PHP_FE(dwavd_stats_scans_array, NULL)
    PHP_FE(dwavd_stats_scans, NULL)
    PHP_FE(dwavd_stats_infcd_array, NULL)
    PHP_FE(dwavd_stats_infcd, NULL)
    PHP_FE(dwavd_stats_sts_state, NULL)
    PHP_FE(dwavd_stats_sts_state_array, NULL)
    PHP_FE(dwavd_stats_traffic, NULL)
    PHP_FE(dwavd_stats_traffic_array, NULL)
    PHP_FE(dwavd_virus, NULL)
    PHP_FE(dwavd_virus_array, NULL)
    PHP_FE(dwavd_infcd_obj, NULL)
    PHP_FE(dwavd_infcd_obj_array, NULL)
    PHP_FE(dwavd_grp_send_message, NULL)
    PHP_FE(dwavd_st_send_message, NULL)
    PHP_FE(dwavd_grp_init, NULL)
    PHP_FE(dwavd_trf_init, NULL)
    PHP_FE(dwavd_adm_init, NULL)
    PHP_FE(dwavd_st_init, NULL)
    {NULL, NULL, NULL} /* Must be the last line in dwavd_functions[] */
};

zend_module_entry dwavd_module_entry = {
    STANDARD_MODULE_HEADER,
    "dwavd",
    dwavd_functions,
    PHP_MINIT(dwavd),
    PHP_MSHUTDOWN(dwavd),
    PHP_RINIT(dwavd),
    PHP_RSHUTDOWN(dwavd),
    PHP_MINFO(dwavd),
    DWAVD_EXTENSION_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_DWAVD

ZEND_GET_MODULE(dwavd)
#endif


/** ======= Constants ======= */

PHP_MINIT_FUNCTION(dwavd) {
    ZEND_INIT_MODULE_GLOBALS(dwavd, _dwavd_globals_init, NULL);
    
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_INFECTED_FILE", DWAVDAPI_INFECTED_FILE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_VIRUS_MODIFICATION_PROGRAM", DWAVDAPI_VIRUS_MODIFICATION_PROGRAM, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_UNKNOWN_VIRUS_PROGRAM", DWAVDAPI_UNKNOWN_VIRUS_PROGRAM, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_ADWARE_PROGRAM", DWAVDAPI_ADWARE_PROGRAM, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_DIALER_PROGRAM", DWAVDAPI_DIALER_PROGRAM, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_JOKE_PROGRAM", DWAVDAPI_JOKE_PROGRAM, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_RISKWARE_PROGRAM", DWAVDAPI_RISKWARE_PROGRAM, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_HACKTOOL_PROGRAM", DWAVDAPI_HACKTOOL_PROGRAM, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_IS_CURED", DWAVDAPI_FILE_IS_CURED, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_INCURABLE", DWAVDAPI_FILE_INCURABLE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_READ_ERR", DWAVDAPI_FILE_READ_ERR, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_WRITE_ERR", DWAVDAPI_FILE_WRITE_ERR, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_PACKED_ERR", DWAVDAPI_FILE_PACKED_ERR, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_PASSWORD_PROTECTED", DWAVDAPI_FILE_PASSWORD_PROTECTED, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_DELETED", DWAVDAPI_FILE_DELETED, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_MOVED", DWAVDAPI_FILE_MOVED, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_RENAMED", DWAVDAPI_FILE_RENAMED, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_IS_ARCHIVE", DWAVDAPI_FILE_IS_ARCHIVE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_CRC_ERROR", DWAVDAPI_FILE_CRC_ERROR, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_HEADER_CRC_ERROR", DWAVDAPI_HEADER_CRC_ERROR, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_ARC_NO_MORE_FILES", DWAVDAPI_ARC_NO_MORE_FILES, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_ARC_NO_MEMORY", DWAVDAPI_ARC_NO_MEMORY, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_ARC_INCOMPLETE", DWAVDAPI_ARC_INCOMPLETE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_ARC_PACKED_ERR", DWAVDAPI_ARC_PACKED_ERR, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_LOCKED", DWAVDAPI_FILE_LOCKED, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_SHUTDOWN", DWAVDAPI_FILE_SHUTDOWN, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_FILE_IGNORED", DWAVDAPI_FILE_IGNORED, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_DWAVDAPI_ENGINE_CRASHED", DWAVDAPI_ENGINE_CRASHED, CONST_CS | CONST_PERSISTENT);
    
    /** Connection errors

        DWAVD_ERR_HTTP_URL_MALFORMAT - 
        DWAVD_ERR_HTTP_COULDNT_RESOLVE_HOST - 
        DWAVD_ERR_HTTP_COULDNT_CONNECT - 
        DWAVD_ERR_HTTP_ACCESS_DENIED - 
        DWAVD_ERR_HTTP_SERVER_INTERNAL_ERROR - 
        DWAVD_ERR_HTTP_CONNECTION_TIMEOUT - 
        DWAVD_ERR_HTTP_LOGIN_DENIED - 
        DWAVD_ERR_DOC_UNEXPECTED_FORMAT - 
        DWAVD_ERR_HTTP_SSL_CONNECT_ERROR - 
        DWAVD_ERR_HTTP_SSL_CERTPROBLEM - 
        DWAVD_ERR_HTTP_SSL_CIPHER - 
        DWAVD_ERR_HTTP_SSL_CACERT - 
        DWAVD_ERR_HTTP_PEER_FAILED_VERIFICATION - 
        DWAVD_ERR_SERVER_RETURN_ERROR -  
     */
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_URL_MALFORMAT", DWAVDAPI_ERR_HTTP_URL_MALFORMAT, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_COULDNT_RESOLVE_HOST", DWAVDAPI_ERR_HTTP_COULDNT_RESOLVE_HOST, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_COULDNT_CONNECT", DWAVDAPI_ERR_HTTP_COULDNT_CONNECT, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_ACCESS_DENIED", DWAVDAPI_ERR_HTTP_ACCESS_DENIED, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_SERVER_INTERNAL_ERROR", DWAVDAPI_ERR_HTTP_SERVER_INTERNAL_ERROR, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_CONNECTION_TIMEOUT", DWAVDAPI_ERR_HTTP_CONNECTION_TIMEOUT, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_LOGIN_DENIED", DWAVDAPI_ERR_HTTP_LOGIN_DENIED, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_DOC_UNEXPECTED_FORMAT", DWAVDAPI_ERR_DOC_UNEXPECTED_FORMAT, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_SSL_CONNECT_ERROR", DWAVDAPI_ERR_HTTP_SSL_CONNECT_ERROR, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_SSL_CERTPROBLEM", DWAVDAPI_ERR_HTTP_SSL_CERTPROBLEM, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_SSL_CIPHER", DWAVDAPI_ERR_HTTP_SSL_CIPHER, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_SSL_CACERT", DWAVDAPI_ERR_HTTP_SSL_CACERT, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_HTTP_PEER_FAILED_VERIFICATION", DWAVDAPI_ERR_HTTP_PEER_FAILED_VERIFICATION,  CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_ERR_SERVER_RETURN_ERROR", DWAVDAPI_ERR_SERVER_RETURN_ERROR,  CONST_CS | CONST_PERSISTENT);

    /** Administrators types

        DWAVD_ADMIN_TYPE_GLOBAL_ADMIN - 
        DWAVD_ADMIN_TYPE_GROUP_ADMIN - 

     */
    REGISTER_LONG_CONSTANT("DWAVD_ADMIN_TYPE_GLOBAL_ADMIN", DWAVDAPI_ADMIN_TYPE_GLOBAL_ADMIN, CONST_CS | CONST_PERSISTENT);    
    REGISTER_LONG_CONSTANT("DWAVD_ADMIN_TYPE_GROUP_ADMIN", DWAVDAPI_ADMIN_TYPE_GROUP_ADMIN, CONST_CS | CONST_PERSISTENT);

    /** AV Components

        DWAVD_COMPONENT_DRWEB32W - 
        DWAVD_COMPONENT_SPIDER9X - 
        DWAVD_COMPONENT_SPIDERNT - 
        DWAVD_COMPONENT_SPIDERMAILHOME - 
        DWAVD_COMPONENT_SPIDERNTSERVER - 
        DWAVD_COMPONENT_SPIDERGATE - 
        DWAVD_COMPONENT_DWPROT - 
        DWAVD_COMPONENT_VADERETRO - 
        DWAVD_COMPONENT_OUTLOOK - 
        DWAVD_COMPONENT_FIREWALL - 
     */
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_DRWEB32W", DWAVDAPI_COMPONENT_DRWEB32W, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_SPIDER9X", DWAVDAPI_COMPONENT_SPIDER9X, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_SPIDERNT", DWAVDAPI_COMPONENT_SPIDERNT, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_SPIDERMAILHOME", DWAVDAPI_COMPONENT_SPIDERMAILHOME, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_SPIDERNTSERVER", DWAVDAPI_COMPONENT_SPIDERNTSERVER, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_SPIDERGATE", DWAVDAPI_COMPONENT_SPIDERGATE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_DWPROT", DWAVDAPI_COMPONENT_DWPROT, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_VADERETRO", DWAVDAPI_COMPONENT_VADERETRO, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_OUTLOOK", DWAVDAPI_COMPONENT_OUTLOOK, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_FIREWALL", DWAVDAPI_COMPONENT_FIREWALL, CONST_CS | CONST_PERSISTENT);
    
    /** AV-agent packages types

        DWAVD_PACKAGE_TYPE_WINDOWS - Windows
        DWAVD_PACKAGE_TYPE_ANDROID - Android
        DWAVD_PACKAGE_TYPE_MACOS - MacOS
        DWAVD_PACKAGE_TYPE_LINUX_32 - Linux 32bit
        DWAVD_PACKAGE_TYPE_LINUX_64 - Linux 64bit
     */
    REGISTER_LONG_CONSTANT("DWAVD_PACKAGE_TYPE_WINDOWS", DWAVDAPI_PACKAGE_TYPE_WINDOWS, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_PACKAGE_TYPE_ANDROID", DWAVDAPI_PACKAGE_TYPE_ANDROID, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_PACKAGE_TYPE_MACOS", DWAVDAPI_PACKAGE_TYPE_MACOS, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_PACKAGE_TYPE_LINUX_32", DWAVDAPI_PACKAGE_TYPE_LINUX_32, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_PACKAGE_TYPE_LINUX_64", DWAVDAPI_PACKAGE_TYPE_LINUX_64, CONST_CS | CONST_PERSISTENT);
    
    /** AV Components states 

        DWAVD_COMPONENT_DISABLED - 
        DWAVD_COMPONENT_OPTIONAL - 
        DWAVD_COMPONENT_REQUIRED - 
     */
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_DISABLED", DWAVDAPI_COMPONENT_DISABLED, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_OPTIONAL", DWAVDAPI_COMPONENT_OPTIONAL, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("DWAVD_COMPONENT_REQUIRED", DWAVDAPI_COMPONENT_REQUIRED, CONST_CS | CONST_PERSISTENT);
    
    /** Built-in tariff groups

        DWAVD_TARIFF_ID_CLASSIC - 
        DWAVD_TARIFF_ID_STANDART - 
        DWAVD_TARIFF_ID_PREMIUM - 
        DWAVD_TARIFF_ID_PREMIUM_SRV - 
        DWAVD_TARIFF_ID_MOBILE - 
        DWAVD_TARIFF_ID_FREE - 
        DWAVD_TARIFF_ID_J_CLASSIC - 
        DWAVD_TARIFF_ID_J_STANDART - 
        DWAVD_TARIFF_ID_J_PREMIUM - 
     */
    REGISTER_STRING_CONSTANT("DWAVD_TARIFF_ID_CLASSIC", DWAVDAPI_TARIFF_ID_CLASSIC, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVD_TARIFF_ID_STANDART", DWAVDAPI_TARIFF_ID_STANDART, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVD_TARIFF_ID_PREMIUM", DWAVDAPI_TARIFF_ID_PREMIUM, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVD_TARIFF_ID_PREMIUM_SRV", DWAVDAPI_TARIFF_ID_PREMIUM_SRV, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVD_TARIFF_ID_FREE", DWAVDAPI_TARIFF_ID_FREE, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVD_TARIFF_ID_MOBILE", DWAVDAPI_TARIFF_ID_MOBILE, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVD_TARIFF_ID_J_CLASSIC", DWAVDAPI_TARIFF_ID_J_CLASSIC, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVD_TARIFF_ID_J_STANDART", DWAVDAPI_TARIFF_ID_J_STANDART, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVD_TARIFF_ID_J_PREMIUM", DWAVDAPI_TARIFF_ID_J_PREMIUM, CONST_CS | CONST_PERSISTENT);
    
   REGISTER_STRING_CONSTANT("DWAVD_SERVER_TASK_SCC_UNKNOWN_STATIONS", DWAVDAPI_SERVER_TASK_SCC_UNKNOWN_STATIONS, CONST_CS | CONST_PERSISTENT);
    
    /** System groups IDs 
     
        DWAVDAPI_GROUP_ID_EVERYONE - 
        DWAVDAPI_GROUP_ID_OPERATING_SYSTEM - 
        DWAVDAPI_GROUP_ID_STATUS - 
        DWAVDAPI_GROUP_ID_TRANSPORT - 
        DWAVDAPI_GROUP_ID_UNGROUPED - 
     */
    REGISTER_STRING_CONSTANT("DWAVDAPI_GROUP_ID_EVERYONE", DWAVDAPI_GROUP_ID_EVERYONE, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVDAPI_GROUP_ID_OPERATING_SYSTEM", DWAVDAPI_GROUP_ID_OPERATING_SYSTEM, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVDAPI_GROUP_ID_STATUS", DWAVDAPI_GROUP_ID_STATUS, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVDAPI_GROUP_ID_TRANSPORT", DWAVDAPI_GROUP_ID_TRANSPORT, CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("DWAVDAPI_GROUP_ID_UNGROUPED", DWAVDAPI_GROUP_ID_UNGROUPED, CONST_CS | CONST_PERSISTENT);
    
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_API_BUILD)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_API_VERSION)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_GROUPS_CUSTOM)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_GROUPS_SYSTEM)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_GROUPS_TOTAL)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_HOST)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_OS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_PLATFORM)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATIONS_AVAILABLE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATIONS_LICENSED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATIONS_TOTAL)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_TARIFFS_TOTAL)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_VERSION)   
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_UPTIME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_ACTIVATED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_ANTISPAM)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_CLIENTS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_CREATED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_DEALER)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_DEALER_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_EXPIRES)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_MD5)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_PRODUCTS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_SERVERS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_SN)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_USER)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRVKEY_USER_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATS_TARIFFS_TOTAL)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATS_GROUPS_TOTAL)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATS_GROUPS_CUSTOM)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATS_GROUPS_SYSTEM)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATS_STATIONS_TOTAL)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATS_STATIONS_STATE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATS_SCANS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATS_INFECTIONS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATS_VIRUSES)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SRV_STATS_TRAFFIC)            
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_CREATED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_DESCRIPTION)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_GROUPS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_GROUPS_COUNT)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_LAST_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_LOGIN)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_MIDDLE_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_MODIFIED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_PASSWORD)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_TYPE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ADM_DEL_FROM_GROUPS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_ADMINS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_ADMINS_COUNT)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_CHILD_GROUPS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_CHILD_GROUPS_COUNT)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_CREATED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_DESCRIPTION)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_EMAILS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_EMAILS_COUNT)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_PARENT_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_MODIFIED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_STATIONS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_STATIONS_COUNT)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_TYPE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_RIGHTS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_DEL_EMAILS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_CHILD_GROUPS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_CHILD_GROUPS_COUNT)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_CREATED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_DESCRIPTION)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_PARENT_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_MODIFIED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_STATIONS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_STATIONS_COUNT)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_TYPE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_COMPONENTS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_GRACE_PERIOD)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRF_RIGHTS)                        
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_CODE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_INHERITED_GROUP_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_VALUE)            
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_RHT_CODE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_RHT_INHERITED_GROUP_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_RHT_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_RHT_VALUE)            
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_INSTD_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_INSTD_INSTALLED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_INSTD_SERVER)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_INSTD_PATH)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_RUN_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_RUN_STARTED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_RUN_TYPE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_RUN_USER)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_CMP_RUN_PARAMS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_BASE_FILE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_BASE_VERSION)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_BASE_VIRUSES)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_BASE_CREATED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_PACKAGE_URL)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_PACKAGE_TYPE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_MOD_FILE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_MOD_VERSION)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_MOD_CREATED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_MOD_MODIFIED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_MOD_HASH)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_MOD_SIZE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_MOD_NAME)            
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_OS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_OS_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_COUNTRY)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_COUNTRY_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_COUNTRY_CODE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_GRACE_PERIOD)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_LONGITUDE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_LATITUDE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_CREATED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_MODIFIED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_EXPIRES)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_CITY)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_FLOOR)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_URL)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_CONFIG)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_PASSWORD)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_DESCRIPTION)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_ROOM)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STREET)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_PROVINCE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_DEPARTMENT)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_TARIFF_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_ORGANIZATION)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_PARENT_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_EMAILS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_EMAILS_COUNT)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_DEL_EMAILS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_GROUPS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_GROUPS_COUNT)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_COMPONENTS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_BASES)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_PACKAGES)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_MODULES)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_RIGHTS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_COMPONENTS_RUN)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_COMPONENTS_INSTD)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_BLOCK_BEGINS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_BLOCK_ENDS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_LASTSEEN_TIME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_LASTSEEN_ADDR)            
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATE_ONLINE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATE_DEINSTALLED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATE_BLOCKED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATE_EXPIRED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATE_OFFLINE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATE_ACTIVATED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATE_UNACTIVATED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATE_TOTAL)                   
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATS_SCANS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATS_INFECTIONS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_STATS_VIRUSES)            
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_ST_SCC)   
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SCANS_SIZE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SCANS_FILES)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SCANS_DELETED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SCANS_MOVED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SCANS_CURED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SCANS_ERRORS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SCANS_INFECTED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SCANS_RENAMED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_SCANS_LOCKED)            
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_DELETED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_CURED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_MOVED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_INCURABLE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_LOCKED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_RENAMED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_ERRORS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_IGNORED)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_TOTAL)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRAFFIC_TOTAL)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRAFFIC_IN)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_TRAFFIC_OUT)            
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_VIRUS_NAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_VIRUS_OBJECTS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_VIRUS_OBJECTS_COUNT)            
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_TYPE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_CURE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_TYPE_CODE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_CURE_CODE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_INFECTION_TYPE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_INFECTION_TYPE_CODE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_ORIGINATOR)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_ORIGINATOR_CODE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_PATH)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_OWNER)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_STATION_ID)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_INFCD_OBJ_USERNAME)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_STATS_STATIONS_TOTAL)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_STATS_STATIONS_STATE)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_STATS_SCANS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_STATS_INFECTIONS)
    DWAVD_REGISTER_LONG_CONSTANT(DWAVD_GRP_STATS_VIRUSES)
    
    le_dwavd = zend_register_list_destructors_ex(_dwavd_dtor, NULL, LE_DWAVD_NAME, module_number);
    le_dwavd_srv = zend_register_list_destructors_ex(_dwavd_srv_dtor, NULL, LE_DWAVD_SRV_NAME, module_number);
    le_dwavd_srv_stats = zend_register_list_destructors_ex(_dwavd_srv_stats_dtor, NULL, LE_DWAVD_SRV_STATS_NAME, module_number);
    le_dwavd_srv_key = zend_register_list_destructors_ex(_dwavd_srv_key_dtor, NULL, LE_DWAVD_SRV_KEY_NAME, module_number);
    le_dwavd_adm = zend_register_list_destructors_ex(_dwavd_adm_dtor, NULL, LE_DWAVD_ADM_NAME, module_number);
    le_dwavd_grp = zend_register_list_destructors_ex(_dwavd_grp_dtor, NULL, LE_DWAVD_GRP_NAME, module_number);
    le_dwavd_grp_stats = zend_register_list_destructors_ex(_dwavd_grp_stats_dtor, NULL, LE_DWAVD_GRP_STATS_NAME, module_number);
    le_dwavd_trf = zend_register_list_destructors_ex(_dwavd_grp_dtor, NULL, LE_DWAVD_TRF_NAME, module_number);
    le_dwavd_st = zend_register_list_destructors_ex(_dwavd_st_dtor, NULL, LE_DWAVD_ST_NAME, module_number);
    le_dwavd_st_stats = zend_register_list_destructors_ex(_dwavd_st_stats_dtor, NULL, LE_DWAVD_ST_STATS_NAME, module_number);
    le_dwavd_components_list = zend_register_list_destructors_ex(_dwavd_components_dtor, NULL, LE_DWAVD_COMPONENTS_LST_NAME, module_number);
    le_dwavd_component = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_COMPONENT_NAME, module_number);
    le_dwavd_rights_list = zend_register_list_destructors_ex(_dwavd_rights_dtor, NULL, LE_DWAVD_RIGHTS_LST_NAME, module_number);
    le_dwavd_right = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_RIGHT_NAME, module_number);
    le_dwavd_components_installed_list = zend_register_list_destructors_ex(_dwavd_components_installed_dtor, NULL, LE_DWAVD_COMPONENTS_INSTALLED_LST_NAME, module_number);
    le_dwavd_component_installed = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_COMPONENT_INSTALLED_NAME, module_number);
    le_dwavd_components_running_list = zend_register_list_destructors_ex(_dwavd_components_running_dtor, NULL, LE_DWAVD_COMPONENTS_RUNNING_LST_NAME, module_number);
    le_dwavd_component_running = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_COMPONENT_RUNNING_NAME, module_number);
    le_dwavd_modules_list = zend_register_list_destructors_ex(_dwavd_modules_dtor, NULL, LE_DWAVD_MODULES_LST_NAME, module_number);
    le_dwavd_module = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_MODULE_NAME, module_number);
    le_dwavd_bases_list = zend_register_list_destructors_ex(_dwavd_bases_dtor, NULL, LE_DWAVD_BASES_LST_NAME, module_number);
    le_dwavd_base = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_BASE_NAME, module_number);
    le_dwavd_packages_list = zend_register_list_destructors_ex(_dwavd_packages_dtor, NULL, LE_DWAVD_PACKAGES_LST_NAME, module_number);
    le_dwavd_package = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_PACKAGE_NAME, module_number);
    le_dwavd_trf_list = zend_register_list_destructors_ex(_dwavd_grp_list_dtor, NULL, LE_DWAVD_TRF_LST_NAME, module_number);
    le_dwavd_grp_list = zend_register_list_destructors_ex(_dwavd_grp_list_dtor, NULL, LE_DWAVD_GRP_LST_NAME, module_number);
    le_dwavd_adm_list = zend_register_list_destructors_ex(_dwavd_adm_list_dtor, NULL, LE_DWAVD_ADM_LST_NAME, module_number);    
    le_dwavd_stats_scans = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_STATS_SCANS_NAME, module_number);
    le_dwavd_stats_traffic = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_STATS_TRAFFIC_NAME, module_number);
    le_dwavd_stats_infections = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_STATS_INFECTIONS_NAME, module_number);
    le_dwavd_stations_state = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_STATIONS_STATE_NAME, module_number);    
    le_dwavd_viruses_list = zend_register_list_destructors_ex(_dwavd_viruses_list_dtor, NULL, LE_DWAVD_VIRUSES_LST_NAME, module_number);
    le_dwavd_virus = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_VIRUS_NAME, module_number);
    le_dwavd_infcd_objs_list = zend_register_list_destructors_ex(_dwavd_infcd_objs_dtor, NULL, LE_DWAVD_INFCD_OBJS_LST_NAME, module_number);
    le_dwavd_infcd_obj = zend_register_list_destructors_ex(NULL, NULL, LE_DWAVD_INFCD_OBJ_NAME, module_number);
    
    _dwavd_lst_res[0] = le_dwavd_components_list;
    _dwavd_lst_res[1] = le_dwavd_rights_list;
    _dwavd_lst_res[2] = le_dwavd_components_running_list;
    _dwavd_lst_res[3] = le_dwavd_components_installed_list;
    _dwavd_lst_res[4] = le_dwavd_modules_list;
    _dwavd_lst_res[5] = le_dwavd_bases_list;
    _dwavd_lst_res[6] = le_dwavd_packages_list;
    _dwavd_lst_res[7] = le_dwavd_trf_list;
    _dwavd_lst_res[8] = le_dwavd_grp_list;
    _dwavd_lst_res[9] = le_dwavd_adm_list;
    _dwavd_lst_res[10] = le_dwavd_viruses_list;
    _dwavd_lst_res[11] = le_dwavd_infcd_objs_list;
    _dwavd_lst_res_count = sizeof(_dwavd_lst_res)/sizeof(int);

    REGISTER_INI_ENTRIES();
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(dwavd) {
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

PHP_RINIT_FUNCTION(dwavd) {
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(dwavd) {
    return SUCCESS;
}

PHP_MINFO_FUNCTION(dwavd) {
    php_info_print_table_start();
    if (sapi_module.phpinfo_as_text) {
        php_info_print_table_colspan_header(2, "(c) "DWAVDAPI_COPYRIGHT);
    } else {
        php_info_print_table_colspan_header(2, "&copy "DWAVDAPI_COPYRIGHT);
    }
    php_info_print_table_end();
    php_info_print_table_start();
    php_info_print_table_row(2, "Extension Version", DWAVD_EXTENSION_VERSION);
    php_info_print_table_row(2, "Version of Dr.Web(R) AV-Desk(TM) XML API Library", dwavdapi_version());
    php_info_print_table_row(2, "Supported version of Dr.Web AV-Desk XML API", "3.0.x; 3.1.x");      
    php_info_print_table_row(2, "Site", "http://www.drweb.com");
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();
}


/** ======= Basics ======= */


/** int dwavd_lib_version()

   Returns dwavdapi library version number. */
PHP_FUNCTION(dwavd_lib_version) {
    DWAVD_CHECK_PARAM_COUNT(0)
    RETURN_LONG(DWAVDAPI_VERSION_NUM);
}


/** int dwavd_version()

   Returns php-avdesk extension version number. */
PHP_FUNCTION(dwavd_version) {
    DWAVD_CHECK_PARAM_COUNT(0) 
    RETURN_LONG(DWAVD_EXTENSION_VERSION_NUM)
}


/** mixed dwavd_init(string host, int port, string login, string password[, string certificate])

   Initializes a connection resource. 
   Returns initialized resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_init) {
    char *host = DWAVD_G(avdesk_host);
    char *login = DWAVD_G(avdesk_login);
    char *pwd = DWAVD_G(avdesk_password);
    char *crt = DWAVD_G(avdesk_crt);
    long port = DWAVD_G(avdesk_port);
    int host_len = strlen(host);
    int login_len = strlen(login);
    int pwd_len = strlen(pwd);
    int crt_len = strlen(crt);
    char *error = NULL;
    dwavdapi_handle *handle = NULL;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|slsss", &host, &host_len, &port, &login, &login_len, &pwd, &pwd_len, &crt, &crt_len)) {
        return;
    }
        
    handle = dwavdapi_init();
    if (NULL == handle) {
        dwavdapi_strerror(&error, errno);
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to initialize connection resource: %s (%d)", error, errno);
        if (error) {
            free(error);
            error = NULL;
        }
        RETURN_NULL();
    }
    if (host_len == 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Host is not specified");
        RETURN_NULL();
    }
    if (login_len == 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Login is not specified");
        RETURN_NULL();
    }
    if (pwd_len == 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Password is not specified");
        RETURN_NULL();
    }
    if (port <= 0 || port > 65535) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Incorrect port is specified");
        RETURN_NULL();
    }
        
    if (DWAVDAPI_FAILURE == dwavdapi_set_connect_info(handle, host, port, login, pwd)) {
        dwavdapi_strerror(&error, errno);
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to set connection parameters: %s (%d)", error, errno);
        if (NULL != error) {
            free(error);
            error = NULL;
        }
        dwavdapi_destroy(handle);
        RETURN_NULL();
    } 
    if(0 < crt_len) {
        if(DWAVDAPI_FAILURE == dwavdapi_set_connect_ssl_crt(handle, crt)){
            dwavdapi_set_connect_ssl_verify(handle, 1);
            RETURN_FALSE
        }
    } 
    
    ZEND_REGISTER_RESOURCE(return_value, handle, le_dwavd);
}


/** bool dwavd_free(resource res)

   Frees memory allocated for a given connection resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_free) {
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    zval *res = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    if(FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** mixed dwavd_switch_to_debug_mode(string debug_filepath)

   Permanently switches dwavdapi library to debug mode when debug 
   information including server responses is written into a file. */
PHP_FUNCTION(dwavd_switch_to_debug_mode) {
    char *fpath = NULL;
    int fpath_len = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fpath, &fpath_len)==FAILURE) {
	return;
    }
    if (fpath_len==0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Debug file path is not specified");
        RETURN_FALSE
    }
    if (dwavdapi_debug_init(fpath)==DWAVDAPI_FAILURE) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** int dwavd_error(resource connection_res, string &error, int &error_code)

   Returns 1 if a given resource has error information, or 0 otherwise.
   Places resource lastest error description into `error` variable, and error code into `error_code` variable. */
PHP_FUNCTION(dwavd_error) {
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    zval *res = NULL;
    int errcode = errno;
    char *error = NULL;
    zval *ref_errcode = NULL;
    zval *ref_error = NULL;
    unsigned int srv_flag = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzz", &res, &ref_error, &ref_errcode) == FAILURE) {
        return;
    }

    DWAVD_FETCH_HANDLE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    dwavdapi_strerror(&error, errcode);
    
    if(errcode == DWAVDAPI_ERR_SERVER_RETURN_ERROR) {
        if(NULL != error) {
            free(error);
            error = NULL;
        }
        srv_flag = 1;
        dwavdapi_srv_error(handle, &error);
        //errcode = dwavdapi_srv_errno(handle);
    }
    zval_dtor(ref_errcode);
    zval_dtor(ref_error);

    ZVAL_LONG(ref_errcode, errcode); 
    if(NULL == error) {
        ZVAL_NULL(ref_error);       
    } else {
        ZVAL_STRING(ref_error, error, 1);
    }
    
    RETURN_LONG(srv_flag)
}


/** bool dwavd_set_port(resource connection_res, int port)

   Sets AV-Desk webserver port number to be used for connection.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_set_port) {
    long port = 0;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    zval *res = NULL;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &res, &port)) {
        return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    if (port <= 0 || port > 65535) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Incorrect port specified");
        RETURN_FALSE
    }
    if(DWAVDAPI_FAILURE == dwavdapi_set_srv_port(handle, port) ){
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** bool dwavd_set_host(resource connection_res, string host)

   Sets AV-Desk webserver host name or URL to be used for connection.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_set_host) {
    int value_len = 0;
    char *value = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    zval *res = NULL;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &value, &value_len)) {
        return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    if (value_len == 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Host is not specified");
        RETURN_FALSE
    }
    if(DWAVDAPI_FAILURE == dwavdapi_set_srv_host(handle, value) ){
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** bool dwavd_set_login(resource connection_res, string login)

   Sets AV-Desk user login to be used for connection.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_set_login) {
    int value_len = 0;
    char *value = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    zval *res = NULL;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &value, &value_len)) {
        return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    if (value_len == 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Login is not specified");
        RETURN_FALSE
    }
    if(DWAVDAPI_FAILURE == dwavdapi_set_srv_login(handle, value) ){
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** bool dwavd_set_password(resource connection_res, string password)

   Sets AV-Desk user password to be used for connection.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_set_password) {
    int value_len = 0;
    char *value = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    zval *res = NULL;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &value, &value_len)) {
	return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    if (value_len == 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Password is not specified");
        RETURN_FALSE
    }
    if(DWAVDAPI_FAILURE == dwavdapi_set_srv_password(handle, value) ){
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** bool dwavd_set_timeout(resource connection_res, int seconds)

   Sets timeout in seconds to be used for connection with AV-Desk.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_set_timeout) {
    long timeout = 0;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    zval *res = NULL;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &res, &timeout)) {
        return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    if (timeout <= 1) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Incorrect timeout is specified. Only greater than 1 values are accepted");
        RETURN_FALSE
    }
    dwavdapi_set_connect_timeout(handle, timeout);
    RETURN_TRUE
}


/** bool dwavd_set_ssl_crt(resource connection_res, string path)

   Sets path of an SSL certificate to be used for connection with AV-Desk.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_set_ssl_crt) {
    int value_len = 0;
    char *value = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    zval *res = NULL;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &value, &value_len)) {
        return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)

    if(0 < value_len) {
        if (DWAVDAPI_FAILURE == dwavdapi_set_connect_ssl_crt(handle, value)){
            RETURN_FALSE
        }
        dwavdapi_set_connect_ssl_verify(handle, 1);
    } else {
        dwavdapi_set_connect_ssl_verify(handle, 0);
        dwavdapi_set_connect_ssl_crt(handle, NULL);
    }
    RETURN_TRUE
}

/** bool dwavd_set_user_agent(resource connection_res, string user_agent)

   Sets custom USER AGENT to be used for connection with AV-Desk.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_set_user_agent) {
    int value_len = 0;
    char *value = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    zval *res = NULL;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &value, &value_len)) {
        return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)

    if(0 < value_len) {
        if(DWAVDAPI_FAILURE == dwavdapi_set_user_agent(handle, value)){
            RETURN_FALSE
        }
    } else {
        if(DWAVDAPI_FAILURE == dwavdapi_set_user_agent(handle, NULL)){
            RETURN_FALSE
        }
    }
    RETURN_TRUE
}


/** ======= Server info resource ======= */

/** mixed dwavd_srv_get_info(resource connection_res)

   Makes a request to AV-Desk to get server information resource.
   Returns server information resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_srv_get_info) {
    zval *res = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_server *srv = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    if (DWAVDAPI_FAILURE == dwavdapi_srv_get_info(handle, &srv)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, srv, le_dwavd_srv);
}


/** bool dwavd_srv_free(resource server_info_res)

   Frees memory allocated for a given server information resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_srv_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_server *srv = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_SRV_RES_WITH_RETURN_FALSE(srv, Z_RESVAL_P(res), le_dwavd_srv, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** mixed dwavd_srv(resource server_info_res, string param_name)

   Returns a value of a given parameter from server information resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_srv) {
    int rsrc_type = 0;
    int flag = -1;
    zval *res = NULL;
    zval *opt = NULL;
    dwavdapi_server *srv = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_SRV_RES_WITH_RETURN_FALSE(srv, Z_RESVAL_P(res), le_dwavd_srv, rsrc_type)  
        
    if(Z_TYPE_P(opt) == IS_STRING) {
        flag = _dwavd_opt_to_flag(Z_STRVAL_P(opt), _dwavd_srvopt_array, sizeof(_dwavd_srvopt_array)/sizeof(_dwavd_srvopt_array[0]));   
    } else if (Z_TYPE_P(opt) == IS_LONG) {
        flag = Z_LVAL_P(opt);
    } else {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected string or number, got %s", _dwavd_var_type(opt));
        RETURN_FALSE
    } 
    
    switch(flag) {
        case DWAVD_SRV_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_uuid(srv));
        case DWAVD_SRV_OS:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_os(srv));
        case DWAVD_SRV_API_BUILD:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_api_build(srv));
        case DWAVD_SRV_API_VERSION:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_api_version(srv));
        case DWAVD_SRV_VERSION:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_version(srv));
        case DWAVD_SRV_HOST:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_host(srv));
        case DWAVD_SRV_PLATFORM:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_platform(srv));   
        case DWAVD_SRV_GROUPS_CUSTOM:
            RETURN_LONG(dwavdapi_srv_groups_custom(srv));
        case DWAVD_SRV_GROUPS_SYSTEM:
            RETURN_LONG(dwavdapi_srv_groups_system(srv));
        case DWAVD_SRV_GROUPS_TOTAL:
            RETURN_LONG(dwavdapi_srv_groups_total(srv));
        case DWAVD_SRV_TARIFFS_TOTAL:
            RETURN_LONG(dwavdapi_srv_tariffs_total(srv));
        case DWAVD_SRV_STATIONS_TOTAL:
            RETURN_LONG(dwavdapi_srv_stations_total(srv));
        case DWAVD_SRV_STATIONS_AVAILABLE:
            RETURN_LONG(dwavdapi_srv_stations_available(srv));
        case DWAVD_SRV_STATIONS_LICENSED:
            RETURN_LONG(dwavdapi_srv_stations_licensed(srv));
        case DWAVD_SRV_UPTIME:
            RETURN_LONG(dwavdapi_srv_uptime(srv));
                
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_srv_array(resource server_info_res)

   Converts data from server information resource into an array and returns it.
   Returns an array with server resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_srv_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_server *srv = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_SRV_RES_WITH_RETURN_FALSE(srv, Z_RESVAL_P(res), le_dwavd_srv, rsrc_type)
    array_init(return_value);
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "id", dwavdapi_srv_uuid(srv))            
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "host", dwavdapi_srv_host(srv))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "os", dwavdapi_srv_os(srv))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "platform", dwavdapi_srv_platform(srv))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "version", dwavdapi_srv_version(srv))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "api_version", dwavdapi_srv_api_version(srv))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "api_build", dwavdapi_srv_api_build(srv))   
    add_assoc_long(return_value, "stations_available", dwavdapi_srv_stations_available(srv));
    add_assoc_long(return_value, "stations_licensed", dwavdapi_srv_stations_licensed(srv));
    add_assoc_long(return_value, "stations_total", dwavdapi_srv_stations_total(srv));
    add_assoc_long(return_value, "groups_custom", dwavdapi_srv_groups_custom(srv));
    add_assoc_long(return_value, "groups_system", dwavdapi_srv_groups_system(srv));
    add_assoc_long(return_value, "groups_total", dwavdapi_srv_groups_total(srv));
    add_assoc_long(return_value, "tariffs_total", dwavdapi_srv_tariffs_total(srv));
    add_assoc_long(return_value, "uptime", dwavdapi_srv_uptime(srv));
}



/** ======= Stats: Server resource ======= */

/** mixed dwavd_srv_get_stats(resource connection_res[, int ts_from[, int ts_till[, int virus_limit]]])

   Makes a request to AV-Desk to get server statistics resource.
   Returns server statistics resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_srv_get_stats) {
    zval *res = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_server_statistics *stats = NULL;
    long till = 0;
    long from = 0;
    long top_viruses = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlll", &res, &from, &till, &top_viruses) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)

    DWAVD_CHECK_TIME(from, from)
    DWAVD_CHECK_TIME(till, till)
            
    if(0 > top_viruses) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Positive number is expected for `top_viruses' argument.");
        RETURN_NULL();
    }
            
    if (DWAVDAPI_FAILURE == dwavdapi_srv_get_statistics(handle, &stats, from, till, top_viruses)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, stats, le_dwavd_srv_stats);
}


/** bool dwavd_stats_free(resource server_stats_res)

   Frees memory allocated for a given server statistics resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_srv_stats_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_server_statistics *stats = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_SRV_STATS_RES_WITH_RETURN_FALSE(stats, Z_RESVAL_P(res), le_dwavd_srv_stats, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** mixed dwavd_srv_stats(resource server_stats_res, string param_name)

   Returns a value of a given parameter from server statistics resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_srv_stats) {
    int rsrc_type = 0;
    int flag = -1;
    zval *res = NULL;
    zval *opt = NULL;
    dwavdapi_server_statistics *stats = NULL;
    dwavdapi_statistics_scans scans;
    dwavdapi_statistics_infections infections;
    dwavdapi_statistics_stations_state state;
    dwavdapi_statistics_traffic traffic;
    dwavdapi_list *list = NULL;
    dwavdapi_list *n_list = NULL;
    int rsrc = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_SRV_STATS_RES_WITH_RETURN_FALSE(stats, Z_RESVAL_P(res), le_dwavd_srv_stats, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_srv_stats_opt_array) 
    
    switch(flag) {
        case DWAVD_SRV_STATS_TARIFFS_TOTAL:
            RETURN_LONG(dwavdapi_srv_statistics_tariffs_total(stats));
        case DWAVD_SRV_STATS_GROUPS_TOTAL:
            RETURN_LONG(dwavdapi_srv_statistics_groups_total(stats));
        case DWAVD_SRV_STATS_GROUPS_CUSTOM:
            RETURN_LONG(dwavdapi_srv_statistics_groups_custom(stats));
        case DWAVD_SRV_STATS_GROUPS_SYSTEM:
            RETURN_LONG(dwavdapi_srv_statistics_groups_system(stats));
        case DWAVD_SRV_STATS_STATIONS_TOTAL:
            RETURN_LONG(dwavdapi_srv_statistics_stations_total(stats));
        case DWAVD_SRV_STATS_STATIONS_STATE:
            state = dwavdapi_srv_statistics_stations_state(stats);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, &state, le_dwavd_stations_state)
            RETURN_RESOURCE(rsrc);     
        case DWAVD_SRV_STATS_SCANS:
            scans = dwavdapi_srv_statistics_scans(stats);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, &scans, le_dwavd_stats_scans)
            RETURN_RESOURCE(rsrc);     
        case DWAVD_SRV_STATS_INFECTIONS:
            infections = dwavdapi_srv_statistics_infections(stats);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, &infections, le_dwavd_stats_infections)
            RETURN_RESOURCE(rsrc); 
        case DWAVD_SRV_STATS_TRAFFIC:
            traffic = dwavdapi_srv_statistics_traffic(stats);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, &traffic, le_dwavd_stats_traffic)
            RETURN_RESOURCE(rsrc); 
        case DWAVD_SRV_STATS_VIRUSES:
            list = dwavdapi_srv_statistics_viruses_list(stats);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_virus_ctor);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_viruses_list)
            RETURN_RESOURCE(rsrc); 
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_srv_stats_array(resource server_stats_res)

   Converts data from server statistics resource into an array and returns it.
   Returns an array with server stats resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_srv_stats_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_server_statistics *stats = NULL;
    dwavdapi_statistics_scans scans;
    dwavdapi_statistics_infections infections;
    dwavdapi_statistics_stations_state state;
    dwavdapi_statistics_traffic traffic;
    dwavdapi_list *list = NULL;
    zval *list_array = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_SRV_STATS_RES_WITH_RETURN_FALSE(stats, Z_RESVAL_P(res), le_dwavd_srv_stats, rsrc_type)
    array_init(return_value);
    
    add_assoc_long(return_value, "stations_total", dwavdapi_srv_statistics_stations_total(stats));
    add_assoc_long(return_value, "groups_custom", dwavdapi_srv_statistics_groups_custom(stats));
    add_assoc_long(return_value, "groups_system", dwavdapi_srv_statistics_groups_system(stats));
    add_assoc_long(return_value, "groups_total", dwavdapi_srv_statistics_groups_total(stats));
    add_assoc_long(return_value, "tariffs_total", dwavdapi_srv_statistics_tariffs_total(stats));
    
    state = dwavdapi_srv_statistics_stations_state(stats);
    _dwavd_stats_array(&array, &state);
    add_assoc_zval(return_value, "stations_state", array);
    
    scans = dwavdapi_srv_statistics_scans(stats);
    _dwavd_scans_array(&array, &scans);
    add_assoc_zval(return_value, "scans", array);
    
    infections = dwavdapi_srv_statistics_infections(stats);
    _dwavd_infcd_array(&array, &infections);
    add_assoc_zval(return_value, "infections", array);
    
    traffic = dwavdapi_srv_statistics_traffic(stats);
    _dwavd_traffic_array(&array, &traffic);
    add_assoc_zval(return_value, "traffic", array);
    
    list = dwavdapi_srv_statistics_viruses_list(stats);
    _dwavd_res_list_to_array(&list_array TSRMLS_CC, list, le_dwavd_viruses_list);
    add_assoc_zval(return_value, "viruses", list_array);
}



/** ======= Server key resource ======= */

/** mixed dwavd_srv_key_get_info(resource connection_res)

   Makes a request to AV-Desk to get server key information resource.
   Returns server information resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_srv_key_get_info) {
    zval *res = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_server_key *key = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    if (DWAVDAPI_FAILURE == dwavdapi_srv_get_key_info(handle, &key)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, key, le_dwavd_srv_key);
}


/** bool dwavd_srv_key_free(resource server_key_info_res)

   Frees memory allocated for a given server key information resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_srv_key_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_server_key *key = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_SRVKEY_RES_WITH_RETURN_FALSE(key, Z_RESVAL_P(res), le_dwavd_srv_key, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** mixed dwavd_srv_key(resource server_key_info_res, string param_name)

   Returns a value of a given parameter from server key information resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_srv_key) {
    int rsrc_type = 0;
    int flag = -1;
    zval *res = NULL;
    zval *opt = NULL;
    dwavdapi_server_key *key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_SRVKEY_RES_WITH_RETURN_FALSE(key, Z_RESVAL_P(res), le_dwavd_srv_key, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_srvkeyopt_array) 
            
    switch(flag) {
        case DWAVD_SRVKEY_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_key_uuid(key));
        case DWAVD_SRVKEY_DEALER_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_key_dealer_name(key));
        case DWAVD_SRVKEY_USER_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_key_user_name(key));
        case DWAVD_SRVKEY_SN:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_key_sn(key));
        case DWAVD_SRVKEY_MD5:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_srv_key_md5(key));
        case DWAVD_SRVKEY_ACTIVATED:
            RETURN_LONG(dwavdapi_srv_key_activated_time(key));
        case DWAVD_SRVKEY_CREATED:
            RETURN_LONG(dwavdapi_srv_key_created_time(key));
        case DWAVD_SRVKEY_CLIENTS:
            RETURN_LONG(dwavdapi_srv_key_clients(key));
        case DWAVD_SRVKEY_ANTISPAM:
            RETURN_LONG(dwavdapi_srv_key_antispam(key));
        case DWAVD_SRVKEY_DEALER:
            RETURN_LONG(dwavdapi_srv_key_dealer(key));
        case DWAVD_SRVKEY_EXPIRES:
            RETURN_LONG(dwavdapi_srv_key_expires_time(key));
        case DWAVD_SRVKEY_PRODUCTS:
            RETURN_LONG(dwavdapi_srv_key_products(key));
        case DWAVD_SRVKEY_SERVERS:
            RETURN_LONG(dwavdapi_srv_key_servers(key));
        case DWAVD_SRVKEY_USER:
            RETURN_LONG(dwavdapi_srv_key_user(key)); 
    }
    DWAVD_UNKNOWN_OPTION(opt)
}

/** bool dwavd_srv_run_task(resource connection_res, string task_id)

   Makes a request to AV-Desk to run a task on server.
   `task_id` - Task ID. See constants DWAVD_SERVER_TASK_SCC_*.
   Returns true on success, or false otherwise. 
 */
PHP_FUNCTION(dwavd_srv_run_task) {
    int rsrc_type = 0;
    zval *res = NULL;
    char *id = NULL;
    int id_len = 0;
    dwavdapi_handle *handle = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &id, &id_len) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
            
    if (0 == id_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Task ID is not specified");
        RETURN_FALSE
    }

    if (DWAVDAPI_SUCCESS == dwavdapi_srv_run_task(handle, id)) {
        RETURN_TRUE
    }
    RETURN_FALSE
}

/** mixed dwavd_srv_key_array(resource server_key_info_res)

   Converts data from server key information resource into an array and returns it.
   Returns an array with server resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_srv_key_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_server_key *key = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
	return;
    }
    DWAVD_FETCH_SRVKEY_RES_WITH_RETURN_FALSE(key, Z_RESVAL_P(res), le_dwavd_srv_key, rsrc_type)
    array_init(return_value);
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "id", dwavdapi_srv_key_uuid(key))            
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "md5", dwavdapi_srv_key_md5(key))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "dealer_name", dwavdapi_srv_key_dealer_name(key))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "user_name", dwavdapi_srv_key_user_name(key))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "sn", dwavdapi_srv_key_sn(key))  
    add_assoc_long(return_value, "created", dwavdapi_srv_key_created_time(key));
    add_assoc_long(return_value, "expires", dwavdapi_srv_key_expires_time(key));
    add_assoc_long(return_value, "activated", dwavdapi_srv_key_activated_time(key));
    add_assoc_long(return_value, "products", dwavdapi_srv_key_products(key));
    add_assoc_long(return_value, "clients", dwavdapi_srv_key_clients(key));
    add_assoc_long(return_value, "user", dwavdapi_srv_key_user(key));
    add_assoc_long(return_value, "dealer", dwavdapi_srv_key_dealer(key));
    add_assoc_long(return_value, "antispam", dwavdapi_srv_key_antispam(key));
    add_assoc_long(return_value, "servers", dwavdapi_srv_key_servers(key));
}



/** ======= Administrator info resource ======= */

/** mixed dwavd_adm_init()

   Initializes new empty administrator resource.
   Returns initialized resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_adm_init) {
    char *error = NULL;
    dwavdapi_admin *adm = NULL;
    
    DWAVD_CHECK_PARAM_COUNT(0)
    
    adm = dwavdapi_admin_init();
    if (NULL == adm) {
        dwavdapi_strerror(&error, errno);
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot initialize resource: %s (%d)", error, errno);
        if (error) {
            free(error);
            error = NULL;
        }
        RETURN_NULL();
    }    
    ZEND_REGISTER_RESOURCE(return_value, adm, le_dwavd_adm);
}


/** mixed dwavd_adm_get_info(resource connection_res)

   Makes a request to AV-Desk to get administrator information resource.
   Returns group information resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_adm_get_info) {
    zval *res = NULL;
    char *login = NULL;
    int login_len = 0;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_admin *adm = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &login, &login_len) == FAILURE) {
        return;
    }    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)    
    if(0 == login_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Administrator login is not specified");
        RETURN_NULL();
    }            
    if (DWAVDAPI_FAILURE == dwavdapi_admin_get_info(handle, &adm, login)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, adm, le_dwavd_adm);
}


/** bool dwavd_adm_free(resource adm_info_res)

   Frees memory allocated for a given administrator information resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_adm_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_admin *adm = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_ADM_RES_WITH_RETURN_FALSE(adm, Z_RESVAL_P(res), le_dwavd_adm, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** mixed dwavd_adm(resource adm_info_res, string param_name)

   Returns a value of a given parameter from administrator resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_adm) {
    int rsrc_type = 0;
    int flag = -1;
    zval *res = NULL;
    zval *opt = NULL;
    zval *groups = NULL;
    dwavdapi_admin *adm = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_ADM_RES_WITH_RETURN_FALSE(adm, Z_RESVAL_P(res), le_dwavd_adm, rsrc_type)
        
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_admopt_array)
            
    switch(flag) {
        case DWAVD_ADM_DESCRIPTION:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_admin_description(adm));
        case DWAVD_ADM_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_admin_id(adm));
        case DWAVD_ADM_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_admin_name(adm));
        case DWAVD_ADM_LAST_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_admin_last_name(adm));
        case DWAVD_ADM_MIDDLE_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_admin_middle_name(adm));
        case DWAVD_ADM_LOGIN:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_admin_login(adm));
        case DWAVD_ADM_PASSWORD:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_admin_password(adm));
        case DWAVD_ADM_CREATED:
            RETURN_LONG(dwavdapi_admin_created_time(adm));
        case DWAVD_ADM_GROUPS_COUNT:
            RETURN_LONG(dwavdapi_admin_groups_count(adm));
        case DWAVD_ADM_MODIFIED:
            RETURN_LONG(dwavdapi_admin_modified_time(adm));
        case DWAVD_ADM_TYPE:
            RETURN_LONG(dwavdapi_admin_type(adm));
        case DWAVD_ADM_GROUPS:
            _dwavd_carray_to_phparray(&groups, (const char **)dwavdapi_admin_groups_array(adm), dwavdapi_admin_groups_count(adm));
            RETURN_ZVAL(groups, 1, 0);
        case DWAVD_ADM_LIMITED_RIGHTS:
            RETURN_BOOL(dwavdapi_admin_has_limited_rights(adm));
        case DWAVD_ADM_READ_ONLY:
            RETURN_BOOL(dwavdapi_admin_is_readonly(adm));
    }
    DWAVD_UNKNOWN_OPTION(opt)    
}


/** mixed dwavd_adm_array(resource adm_info_res)

   Converts data from administrator information resource into an array and returns it.
   Returns an array with administrator resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_adm_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *groups = NULL;
    dwavdapi_admin *adm = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_ADM_RES_WITH_RETURN_FALSE(adm, Z_RESVAL_P(res), le_dwavd_adm, rsrc_type)
    array_init(return_value);
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "id", dwavdapi_admin_id(adm))            
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "login", dwavdapi_admin_login(adm))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "password", dwavdapi_admin_password(adm))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "name", dwavdapi_admin_name(adm))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "last_name", dwavdapi_admin_last_name(adm))  
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "middle_name", dwavdapi_admin_middle_name(adm)) 
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "description", dwavdapi_admin_description(adm)) 
    add_assoc_long(return_value, "type", dwavdapi_admin_type(adm));
    add_assoc_bool(return_value, "readonly", dwavdapi_admin_is_readonly(adm));
    add_assoc_bool(return_value, "limited_rights", dwavdapi_admin_has_limited_rights(adm));
    add_assoc_long(return_value, "created", dwavdapi_admin_created_time(adm));
    add_assoc_long(return_value, "modified", dwavdapi_admin_modified_time(adm));
    add_assoc_long(return_value, "groups_count", dwavdapi_admin_groups_count(adm));
    _dwavd_carray_to_phparray(&groups, (const char **)dwavdapi_admin_groups_array(adm), dwavdapi_admin_groups_count(adm));
    add_assoc_zval(return_value, "groups", groups);
}


/* Helper function, used further down. */
static int _dwavd_adm_set(dwavdapi_admin *adm, int flag TSRMLS_DC, zval *val) {    
    HashPosition pos = NULL;
    zval **array_item = NULL;
    char *array_strkey = NULL;
    unsigned int array_strkey_len = 0;
    unsigned long array_numkey = 0;
    int array_key_type = 0;

    switch(flag) {
        case DWAVD_ADM_DESCRIPTION:
            DWAVD_ADM_SET_STRING(adm, description, val, 0)    
        case DWAVD_ADM_ID:
            DWAVD_ADM_SET_STRING(adm, id, val, 0) 
        case DWAVD_ADM_NAME:
            DWAVD_ADM_SET_STRING(adm, name, val, 0) 
        case DWAVD_ADM_LAST_NAME:
            DWAVD_ADM_SET_STRING(adm, last_name, val, 0) 
        case DWAVD_ADM_MIDDLE_NAME:
            DWAVD_ADM_SET_STRING(adm, middle_name, val, 0) 
        case DWAVD_ADM_LOGIN:
            DWAVD_ADM_SET_STRING(adm, login, val, 1) 
        case DWAVD_ADM_PASSWORD:
            DWAVD_ADM_SET_STRING(adm, password, val, 0) 
        case DWAVD_ADM_LIMITED_RIGHTS:
            DWAVD_ADM_SET_BOOL(adm, limited_rights, val) 
        case DWAVD_ADM_READ_ONLY:
            DWAVD_ADM_SET_BOOL(adm, readonly, val) 
        case DWAVD_ADM_GROUPS: {
            DWAVD_EXPECTED_ARRAY_WITH_RET(val, 1)
            for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(val), &pos); 
               SUCCESS == zend_hash_has_more_elements_ex(Z_ARRVAL_P(val), &pos); zend_hash_move_forward_ex(Z_ARRVAL_P(val), &pos)) {
                array_key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(val), &array_strkey, &array_strkey_len, &array_numkey, 0, &pos);
                if(HASH_KEY_IS_LONG != array_key_type) {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key of array must be integer, got `%s'", _dwavd_var_type(*array_item));
                    return 1;
                }
                if(SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(val), (void**)&array_item, &pos)){
                    if(Z_TYPE_PP(array_item) != IS_STRING) {
                        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Value of array must be string, got `%s'", _dwavd_var_type(*array_item));
                        return 1;
                    }
                    if(DWAVDAPI_SUCCESS == dwavdapi_admin_add_to_group(adm, Z_STRVAL_PP(array_item))) {
                        return 0;
                    }
                }
            }
            return 1;
        }
        case DWAVD_ADM_DEL_FROM_GROUPS: {
            DWAVD_EXPECTED_ARRAY_WITH_RET(val, 1)
            for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(val), &pos); 
               SUCCESS == zend_hash_has_more_elements_ex(Z_ARRVAL_P(val), &pos); zend_hash_move_forward_ex(Z_ARRVAL_P(val), &pos)) {
                array_key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(val), &array_strkey, &array_strkey_len, &array_numkey, 0, &pos);
                if(HASH_KEY_IS_LONG != array_key_type) {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key of array must be integer, got `%s'", _dwavd_var_type(*array_item));
                    return 1;
                }
                if(SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(val), (void**)&array_item, &pos)){
                    if(Z_TYPE_PP(array_item) != IS_STRING) {
                        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Value of array must be string, got `%s'", _dwavd_var_type(*array_item));
                        return 1;
                    }
                    if(DWAVDAPI_SUCCESS == dwavdapi_admin_delete_from_group(adm, Z_STRVAL_PP(array_item))) {
                        return 0;
                    }
                }
            }
            return 1;
        }
    }
    return 1;
}


/** bool dwavd_adm_set(resource adm_info_res, string param_name, mixed param_value)

   Sets a value of a given parameter to administrator information resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_adm_set) {    
    dwavdapi_admin *adm = NULL;
    int flag = -1;
    int rsrc_type = 0;
    zval *res = NULL;
    zval *opt = NULL;
    zval *val = NULL;    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzz", &res, &opt, &val) == FAILURE) {
        return;
    }
    DWAVD_FETCH_ADM_RES_WITH_RETURN_FALSE(adm, Z_RESVAL_P(res), le_dwavd_adm, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_admopt_array)
            
    if(_dwavd_adm_set(adm, flag TSRMLS_CC, val) ) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** bool dwavd_adm_set_array(resource adm_info_res, array params)

   Sets administrator information resource parameters according to data from a given params array.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_adm_set_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    HashPosition pos = NULL;
    zval *array = NULL;
    zval **array_item = NULL;
    char *array_strkey = NULL;
    unsigned int array_strkey_len = 0;
    unsigned long array_numkey = 0;
    int array_key_type = 0;
    dwavdapi_admin *adm = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &array) == FAILURE) {
        return;
    }

    DWAVD_FETCH_ADM_RES_WITH_RETURN_FALSE(adm, Z_RESVAL_P(res), le_dwavd_adm, rsrc_type)
    DWAVD_EXPECTED_ARRAY(array)
    DWAVD_ARRAY_EMPTY(Z_ARRVAL_P(array))
            
    for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos); 
       SUCCESS == zend_hash_has_more_elements_ex(Z_ARRVAL_P(array), &pos); zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos)) {
        array_key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(array), &array_strkey, &array_strkey_len, &array_numkey, 0, &pos);
        if(SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void**)&array_item, &pos)){
            if(array_key_type == HASH_KEY_IS_STRING) {
                flag = _dwavd_opt_to_flag(array_strkey, _dwavd_admopt_array, sizeof(_dwavd_admopt_array)/sizeof(_dwavd_admopt_array[0]));
                if(-1 == flag) {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown option: `%s'", array_strkey);
                    RETURN_FALSE
                }
            } else {
                flag = array_numkey;
            }
            if(_dwavd_adm_set(adm, flag TSRMLS_CC, *array_item)) {
                RETURN_FALSE
            }
        }
    }    

    RETURN_TRUE
}


/** mixed dwavd_adm_add(resource connection_res, resource adm_res)

   Makes a request to AV-Desk to add administrator described by a given resource.
   Returns an ID for newly-created administrator, or false otherwise. */
PHP_FUNCTION(dwavd_adm_add) {
    zval *res = NULL;
    zval *res_adm = NULL;
    int rsrc_type = 0;
    int rsrc_adm_type = 0;
    char *login = NULL;
    dwavdapi_handle *handle = NULL;
    dwavdapi_admin *adm = NULL;
   
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &res, &res_adm) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    DWAVD_FETCH_ADM_RES_WITH_RETURN_FALSE(adm, Z_RESVAL_P(res_adm), le_dwavd_adm, rsrc_adm_type)

    if (DWAVDAPI_FAILURE == dwavdapi_admin_add(handle, adm, &login)) {
        RETURN_FALSE
    }
    
    RETVAL_STRING(login, 1);
    free(login);
    return;
}


/** bool dwavd_adm_change(resource connection_res, resource adm_res)

   Makes a request to AV-Desk to update administrator with data from a given resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_adm_change) {
    zval *res = NULL;
    zval *res_adm = NULL;
    int rsrc_type = 0;
    int rsrc_adm_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_admin *adm = NULL;
   
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &res, &res_adm) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    DWAVD_FETCH_ADM_RES_WITH_RETURN_FALSE(adm, Z_RESVAL_P(res_adm), le_dwavd_adm, rsrc_adm_type)

    if (DWAVDAPI_FAILURE == dwavdapi_admin_change(handle, adm)) {
        RETURN_FALSE
    }
    
    RETURN_TRUE
}


/** bool dwavd_adm_delete(resource connection_res, string adm_login)

   Makes a request to AV-Desk to delete administrator with a given login.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_adm_delete) {
    zval *res = NULL;
    int rsrc_type = 0;
    int login_len = 0;
    char *login = NULL;
    dwavdapi_handle *handle = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &login, &login_len) == FAILURE) {
        return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)    
    if(0 == login_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Administrator login is not specified");
        RETURN_FALSE
    }            
    if (DWAVDAPI_FAILURE == dwavdapi_admin_delete(handle, login)) {
        RETURN_FALSE
    }   
    
    RETURN_TRUE
}



/** ======= Administrators list resource ======= */

/** mixed dwavd_adm_get_list(resource connection_res)

   Makes a request to AV-Desk to retrieve a list of administrators.
   Returns administrators list resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_adm_get_list) {
    zval *res = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_list *list = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)            
    if (DWAVDAPI_FAILURE == dwavdapi_admin_get_list(handle, &list)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, list, le_dwavd_adm_list);
}


/** bool dwavd_adm_list_free(resource adm_list_res)

   Frees memory allocated for a given administrators list resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_adm_list_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_list *list = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_ADM_LST_RES_WITH_RETURN_FALSE(list, Z_RESVAL_P(res), le_dwavd_adm_list, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}



/** ======= Group info resource ======= */

/** mixed dwavd_grp_init()

   Initializes new empty group resource.
   Returns initialized resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_grp_init) {
    char *error = NULL;
    dwavdapi_group *grp = NULL;
    
    DWAVD_CHECK_PARAM_COUNT(0)
    
    grp = dwavdapi_group_init();
    if (NULL == grp) {
        dwavdapi_strerror(&error, errno);
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot initialize resource: %s (%d)", error, errno);
        if (error) {
            free(error);
            error = NULL;
        }
        RETURN_NULL();
    }    
    ZEND_REGISTER_RESOURCE(return_value, grp, le_dwavd_grp);
}


/** mixed dwavd_grp_get_info(resource connection_res)

   Makes a request to AV-Desk to get group information resource.
   Returns group information resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_grp_get_info) {
    zval *res = NULL;
    char *id = NULL;
    int id_len = 0;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_group *grp = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &id, &id_len) == FAILURE) {
        return;
    }    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)    
    if(0 == id_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Group ID is not specified");
        RETURN_NULL();
    }            
    if (DWAVDAPI_FAILURE == dwavdapi_group_get_info(handle, &grp, id)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, grp, le_dwavd_grp);
}


/** bool dwavd_grp_free(resource grp_info_res)

   Frees memory allocated for a given group information resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_grp_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_group *grp = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res), le_dwavd_grp, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** mixed dwavd_grp(resource grp_info_res, string param_name)

   Returns a value of a given parameter from group resource.
   On failure returns false. */

DIAGNOSTIC_OFF(deprecated-declarations)
PHP_FUNCTION(dwavd_grp) {
    int rsrc_type = 0;
    int flag = -1;
    zval *res = NULL;
    zval *opt = NULL;
    zval *array = NULL;
    dwavdapi_group *grp = NULL;
    dwavdapi_list *rights_list = NULL;
    dwavdapi_list *n_rights_list = NULL;
    int rsrc_rights = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res), le_dwavd_grp, rsrc_type)        
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_grpopt_array)
            
    switch(flag) {
        case DWAVD_GRP_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_group_id(grp));
        case DWAVD_GRP_PARENT_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_group_parent_id(grp));
        case DWAVD_GRP_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_group_name(grp));
        case DWAVD_GRP_DESCRIPTION:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_group_description(grp));
        case DWAVD_GRP_CREATED:
            RETURN_LONG(dwavdapi_group_created_time(grp));
        case DWAVD_GRP_MODIFIED:
            RETURN_LONG(dwavdapi_group_modified_time(grp));
        case DWAVD_GRP_TYPE:
            RETURN_LONG(dwavdapi_group_type(grp));
        case DWAVD_GRP_CHILD_GROUPS_COUNT:
            RETURN_LONG(dwavdapi_group_child_groups_count(grp));
        case DWAVD_GRP_CHILD_GROUPS:
            _dwavd_carray_to_phparray(&array, (const char **)dwavdapi_group_child_groups_array(grp), dwavdapi_group_child_groups_count(grp));
            RETURN_ZVAL(array, 1, 0);
        case DWAVD_GRP_ADMINS_COUNT:
            RETURN_LONG(dwavdapi_group_admins_count(grp));
        case DWAVD_GRP_ADMINS:
            _dwavd_carray_to_phparray(&array, (const char **)dwavdapi_group_admins_array(grp), dwavdapi_group_admins_count(grp));
            RETURN_ZVAL(array, 1, 0);         
        case DWAVD_GRP_EMAILS_COUNT:            
            RETURN_LONG(dwavdapi_group_emails_count(grp));           
        case DWAVD_GRP_EMAILS:
            _dwavd_carray_to_phparray(&array, (const char **)dwavdapi_group_emails_array(grp), dwavdapi_group_emails_count(grp));
            RETURN_ZVAL(array, 1, 0);         
        case DWAVD_GRP_STATIONS_COUNT:
            RETURN_LONG(dwavdapi_group_stations_count(grp));
        case DWAVD_GRP_STATIONS:
            _dwavd_carray_to_phparray(&array, (const char **)dwavdapi_group_stations_array(grp), dwavdapi_group_stations_count(grp));
            RETURN_ZVAL(array, 1, 0);
        case DWAVD_GRP_RIGHTS:
            rights_list = dwavdapi_group_rights_list(grp);
            dwavdapi_list_ctor(&n_rights_list, rights_list, (dwavdapi_ctor_funct)dwavdapi_right_ctor);
            rsrc_rights = ZEND_REGISTER_RESOURCE(NULL, n_rights_list, le_dwavd_rights_list)
            RETURN_RESOURCE(rsrc_rights);     
    }
    DWAVD_UNKNOWN_OPTION(opt)
}
DIAGNOSTIC_ON(deprecated-declarations)

/** mixed dwavd_grp_array(resource grp_info_res)

   Converts data from group information resource into an array and returns it.
   Returns an array with group resource data on success, or false otherwise. 
 */

DIAGNOSTIC_OFF(deprecated-declarations)
PHP_FUNCTION(dwavd_grp_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *stations = NULL;
    zval *admins = NULL;
    zval *emails = NULL;
    zval *child_groups = NULL;
    dwavdapi_group *grp = NULL;
    zval *rights_array = NULL;
    dwavdapi_list *list = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res), le_dwavd_grp, rsrc_type)
    array_init(return_value);
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "id", dwavdapi_group_id(grp)) 
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "name", dwavdapi_group_name(grp))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "description", dwavdapi_group_description(grp))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "parent_id", dwavdapi_group_parent_id(grp))
            
    list = dwavdapi_group_rights_list(grp);
    _dwavd_res_list_to_array(&rights_array TSRMLS_CC, list, le_dwavd_rights_list);
    add_assoc_zval(return_value, "rights", rights_array);
            
    add_assoc_long(return_value, "type", dwavdapi_group_type(grp));
    add_assoc_long(return_value, "created", dwavdapi_group_created_time(grp));
    add_assoc_long(return_value, "modified", dwavdapi_group_modified_time(grp));
    add_assoc_long(return_value, "child_groups_count", dwavdapi_group_child_groups_count(grp));    
    _dwavd_carray_to_phparray(&child_groups, (const char **)dwavdapi_group_child_groups_array(grp), dwavdapi_group_child_groups_count(grp));
    add_assoc_zval(return_value, "child_groups", child_groups);   
    add_assoc_long(return_value, "emails_count", dwavdapi_group_emails_count(grp));
    _dwavd_carray_to_phparray(&emails, (const char **)dwavdapi_group_emails_array(grp), dwavdapi_group_emails_count(grp));
    add_assoc_zval(return_value, "emails", emails);
    add_assoc_long(return_value, "stations_count", dwavdapi_group_stations_count(grp));
    _dwavd_carray_to_phparray(&stations, (const char **)dwavdapi_group_stations_array(grp), dwavdapi_group_stations_count(grp));
    add_assoc_zval(return_value, "stations", stations);
    add_assoc_long(return_value, "admins_count", dwavdapi_group_admins_count(grp));
    _dwavd_carray_to_phparray(&admins, (const char **)dwavdapi_group_admins_array(grp), dwavdapi_group_admins_count(grp));
    add_assoc_zval(return_value, "admins", admins);
}
DIAGNOSTIC_ON(deprecated-declarations)

/* Helper function, used further down. */
DIAGNOSTIC_OFF(deprecated-declarations)
static int _dwavd_grp_set(dwavdapi_group *grp, int flag TSRMLS_DC, zval *val) {    
    HashPosition pos = NULL;
    zval **array_item = NULL;
    char *array_strkey = NULL;
    unsigned int array_strkey_len = 0;
    unsigned long array_numkey = 0;
    int array_key_type = 0;

    switch(flag) {
        case DWAVD_GRP_ID:
           DWAVD_GRP_SET_STRING(grp, id, val, 0)     
        case DWAVD_GRP_NAME:
           DWAVD_GRP_SET_STRING(grp, name, val, 1)
        case DWAVD_GRP_PARENT_ID:
            DWAVD_GRP_SET_STRING(grp, parent_id, val, 0)
        case DWAVD_GRP_DESCRIPTION:
           DWAVD_GRP_SET_STRING(grp, description, val, 0)
        case DWAVD_GRP_EMAILS:
            DWAVD_EXPECTED_ARRAY_WITH_RET(val, 1)
            for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(val), &pos); 
               SUCCESS == zend_hash_has_more_elements_ex(Z_ARRVAL_P(val), &pos); zend_hash_move_forward_ex(Z_ARRVAL_P(val), &pos)) {
                array_key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(val), &array_strkey, &array_strkey_len, &array_numkey, 0, &pos);
                if(HASH_KEY_IS_LONG != array_key_type) {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key of array must be integer, got `%s'", _dwavd_var_type(*array_item));
                    return 1;
                }
                if(SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(val), (void**)&array_item, &pos)){
                    if(Z_TYPE_PP(array_item) != IS_STRING) {
                        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Value of array must be string, got `%s'", _dwavd_var_type(*array_item));
                        return 1;
                    }                     
                    if(DWAVDAPI_FAILURE == dwavdapi_group_add_email(grp, Z_STRVAL_PP(array_item))) {
                        return 1;
                    }                    
                }
            }
            return 0;
        case DWAVD_GRP_DEL_EMAILS:
            DWAVD_EXPECTED_ARRAY_WITH_RET(val, 1)
            for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(val), &pos); 
               SUCCESS == zend_hash_has_more_elements_ex(Z_ARRVAL_P(val), &pos); zend_hash_move_forward_ex(Z_ARRVAL_P(val), &pos)) {
                array_key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(val), &array_strkey, &array_strkey_len, &array_numkey, 0, &pos);
                if(HASH_KEY_IS_LONG != array_key_type) {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key of array must be integer, got `%s'", _dwavd_var_type(*array_item));
                    return 1;
                }
                if(SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(val), (void**)&array_item, &pos)){
                    if(Z_TYPE_PP(array_item) != IS_STRING) {
                        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Value of array must be string, got `%s'", _dwavd_var_type(*array_item));
                        return 1;
                    }                    
                    if(DWAVDAPI_FAILURE == dwavdapi_group_delete_email(grp, Z_STRVAL_PP(array_item))) {
                        return 1;
                    }                    
                }
            }
            return 0;
    }
    return 1;
}
DIAGNOSTIC_ON(deprecated-declarations)

/** bool dwavd_grp_set(resource grp_info_res, string param_name, mixed param_value)

   Sets a value of a given parameter to group information resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_grp_set) {
    int rsrc_type = 0;
    int flag = -1;
    zval *res = NULL;
    zval *opt = NULL;
    zval *val = NULL;
    dwavdapi_group *grp = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzz", &res, &opt, &val) == FAILURE) {
        return;
    }
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res), le_dwavd_grp, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_grpopt_array)
            
    if(_dwavd_grp_set(grp, flag TSRMLS_CC, val)) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** bool dwavd_grp_set_array(resource grp_info_res, array params)

   Sets group information resource parameters according to data from a given params array.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_grp_set_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    HashPosition pos = NULL;
    zval *array = NULL;
    zval **array_item = NULL;
    char *array_strkey = NULL;
    unsigned int array_strkey_len = 0;
    unsigned long array_numkey = 0;
    int array_key_type = 0;
    dwavdapi_group *grp = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &array) == FAILURE) {
        return;
    }

    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res), le_dwavd_grp, rsrc_type)
    DWAVD_EXPECTED_ARRAY(array)
    DWAVD_ARRAY_EMPTY(Z_ARRVAL_P(array))
            
    for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos); 
       SUCCESS == zend_hash_has_more_elements_ex(Z_ARRVAL_P(array), &pos); zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos)) {
        array_key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(array), &array_strkey, &array_strkey_len, &array_numkey, 0, &pos);
        if(SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void**)&array_item, &pos)){
            if(array_key_type == HASH_KEY_IS_STRING) {
                flag = _dwavd_opt_to_flag(array_strkey, _dwavd_grpopt_array, sizeof(_dwavd_grpopt_array)/sizeof(_dwavd_grpopt_array[0]));
                if(-1 == flag) {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown option: `%s'", array_strkey);
                    RETURN_FALSE
                }
            } else {
                flag = array_numkey;
            }
            if(_dwavd_grp_set(grp, flag TSRMLS_CC, *array_item)) {
                RETURN_FALSE
            }
        }
    }    

    RETURN_TRUE
}


/** mixed dwavd_grp_add(resource connection_res, resource grp_res)

   Makes a request to AV-Desk to add group described by a given resource.
   Returns an ID for newly-created group, or false otherwise. */
PHP_FUNCTION(dwavd_grp_add) {
    zval *res = NULL;
    zval *res_grp = NULL;
    int rsrc_type = 0;
    int rsrc_grp_type = 0;
    char *id = NULL;
    dwavdapi_handle *handle = NULL;
    dwavdapi_group *grp = NULL;
   
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &res, &res_grp) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res_grp), le_dwavd_grp, rsrc_grp_type)

    if (DWAVDAPI_FAILURE == dwavdapi_group_add(handle, grp, &id)) {
        RETURN_FALSE
    }
    
    RETVAL_STRING(id, 1);
    free(id);
    return;
}


/** bool dwavd_grp_change(resource connection_res, resource grp_res)

   Makes a request to AV-Desk to update group with data from a given resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_grp_change) {
    zval *res = NULL;
    zval *res_grp = NULL;
    int rsrc_type = 0;
    int rsrc_grp_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_group *grp = NULL;
   
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &res, &res_grp) == FAILURE) {
        return;
    }
        
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res_grp), le_dwavd_grp, rsrc_grp_type)

    if (DWAVDAPI_FAILURE == dwavdapi_group_change(handle, grp)) {
        RETURN_FALSE
    }
    
    RETURN_TRUE
}


/** bool dwavd_grp_delete(resource connection_res, string grp_id)

   Makes a request to AV-Desk to delete group with a given ID.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_grp_delete) {
    zval *res = NULL;
    int rsrc_type = 0;
    int id_len = 0;
    char *id = NULL;
    dwavdapi_handle *handle = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &id, &id_len) == FAILURE) {
        return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)    
    if(0 == id_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Group ID is not specified");
        RETURN_FALSE
    }            
    if (DWAVDAPI_FAILURE == dwavdapi_group_delete(handle, id)) {
        RETURN_FALSE
    }   
    
    RETURN_TRUE
}


/** bool dwavd_grp_send_message(resource connection_res, string group_id, string message[, string link_text[, string link_url[, string logo_filepath[, string logo_link[, string logo_text]]]])

   Makes a request to AV-Desk to send a message to all station in a given group.
   `message` - Message text which can contain {link} macros to place link given in link parameters to.
   `link_text` - URL description text to be placed in {link} macros.
   `link_url` - URL to be placed in {link} macros.
   `logo_filepath` - An absolute path to a logo .bmp file to show in a message.
   `logo_text` - Description text for a logo.
   `logo_link` - URL to go to on a logo click.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_grp_send_message) {
    int rsrc_type = 0;
    zval *res = NULL;
    char *id = NULL;
    char *logo_file = NULL;
    char *logo_link = NULL;
    char *logo_text = NULL;
    char *link_text = NULL;
    char *link_url = NULL;
    char *message = NULL;
    int message_len = 0;
    int logo_file_len = 0;
    int logo_link_len = 0;
    int link_text_len = 0;
    int link_url_len = 0;
    int logo_text_len = 0;
    int id_len = 0;
    dwavdapi_message *msg = NULL;
    dwavdapi_handle *handle = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|sssss", &res, &id, &id_len, &message, &message_len, &link_text, &link_text_len, &link_url, &link_url_len, &logo_file, &logo_file_len, &logo_link, &logo_link_len, &logo_text, &logo_text_len) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
            
    if (0 == id_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Group ID is not specified");
        RETURN_FALSE
    }

    if (0 == message_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Message is not specified");
        RETURN_FALSE
    }

    msg = dwavdapi_message_init();
    dwavdapi_message_set_msg(msg, message);

    if (0 < link_url_len) {
        dwavdapi_message_set_url(msg, link_url);
    }

    if (0 < link_text_len) {
        dwavdapi_message_set_url_text(msg, link_text);
    }

    if (0 < logo_file_len) {
        dwavdapi_message_set_logo(msg, logo_file);
    }

    if (0 < logo_link_len) {
        dwavdapi_message_set_logo_url(msg, logo_link);
    }

    if (0 < logo_text_len) {
        dwavdapi_message_set_logo_text(msg, logo_text);
    }

    if (DWAVDAPI_SUCCESS == dwavdapi_group_send_message(handle, id, msg)) {
        dwavdapi_message_destroy(msg);
        RETURN_TRUE
    }
    dwavdapi_message_destroy(msg);
    RETURN_FALSE
}


/** ======= Groups list resource ======= */

/** mixed dwavd_grp_get_list(resource connection_res)

   Makes a request to AV-Desk to retrieve a list of groups.
   Returns groups list resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_grp_get_list) {
    zval *res = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_list *list = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    
    if (DWAVDAPI_FAILURE == dwavdapi_group_get_list(handle, &list)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, list, le_dwavd_grp_list);    
}


/** bool dwavd_grp_list_free(resource grp_list_res)

   Frees memory allocated for a given groups list resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_grp_list_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_list *list = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_GRP_LST_RES_WITH_RETURN_FALSE(list, Z_RESVAL_P(res), le_dwavd_grp_list, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}



/** ======= Stats: Group resource ======= */

/** mixed dwavd_grp_get_stats(resource connection_res, string grp_id[, int ts_from[, int ts_till[, int virus_limit]]])

   Makes a request to AV-Desk to get statistics resource for a given group.
   Returns group statistics resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_grp_get_stats) {
    zval *res = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_group_statistics *stats = NULL;
    long till = 0;
    long from = 0;
    long top_viruses = 0;
    char *id;
    int id_len = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rslll", &res, &id, &id_len, &from, &till, &top_viruses) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
            
    if(0 == id_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Group ID is not specified");
        RETURN_NULL();
    }  

    DWAVD_CHECK_TIME(from, from)
    DWAVD_CHECK_TIME(till, till)
            
    if(0 > top_viruses) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Incorrect value of `top_viruses'. Expected positive number, got negative");
        RETURN_NULL();
    }
            
    if (DWAVDAPI_FAILURE == dwavdapi_group_get_statistics(handle, &stats, id, from, till, top_viruses)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, stats, le_dwavd_grp_stats);
}


/** bool dwavd_grp_stats_free(resource grp_stats_res)

   Frees memory allocated for a given group statistics resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_grp_stats_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_group_statistics *stats = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_GRP_STATS_RES_WITH_RETURN_FALSE(stats, Z_RESVAL_P(res), le_dwavd_grp_stats, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** mixed dwavd_grp_stats(resource grp_stats_res, string param_name)

   Returns a value of a given parameter from group statistics resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_grp_stats) {
    int rsrc_type = 0;
    int flag = -1;
    zval *res = NULL;
    zval *opt = NULL;
    dwavdapi_group_statistics *stats = NULL;
    dwavdapi_statistics_scans scans;
    dwavdapi_statistics_infections infections;
    dwavdapi_statistics_stations_state state;
    dwavdapi_list *list = NULL;
    dwavdapi_list *n_list = NULL;
    int rsrc = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_GRP_STATS_RES_WITH_RETURN_FALSE(stats, Z_RESVAL_P(res), le_dwavd_grp_stats, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_grp_stats_opt_array) 
    
    switch(flag) {
        case DWAVD_GRP_STATS_STATIONS_TOTAL:
            RETURN_LONG(dwavdapi_group_statistics_stations_total(stats));
        case DWAVD_GRP_STATS_STATIONS_STATE:
            state = dwavdapi_group_statistics_stations_state(stats);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, &state, le_dwavd_stations_state)
            RETURN_RESOURCE(rsrc);     
        case DWAVD_GRP_STATS_SCANS:
            scans = dwavdapi_group_statistics_scans(stats);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, &scans, le_dwavd_stats_scans)
            RETURN_RESOURCE(rsrc);     
        case DWAVD_GRP_STATS_INFECTIONS:
            infections = dwavdapi_group_statistics_infections(stats);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, &infections, le_dwavd_stats_infections)
            RETURN_RESOURCE(rsrc); 
        case DWAVD_GRP_STATS_VIRUSES:
            list = dwavdapi_group_statistics_viruses_list(stats);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_virus_ctor);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_viruses_list)
            RETURN_RESOURCE(rsrc); 
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_grp_stats_array(resource grp_stats_res)

   Converts data from group statistics resource into an array and returns it.
   Returns an array with group stats resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_grp_stats_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_group_statistics *stats = NULL;
    dwavdapi_statistics_scans scans;
    dwavdapi_statistics_infections infections;
    dwavdapi_statistics_stations_state state;
    dwavdapi_list *list = NULL;
    zval *list_array = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_GRP_STATS_RES_WITH_RETURN_FALSE(stats, Z_RESVAL_P(res), le_dwavd_grp_stats, rsrc_type)
    array_init(return_value);
    
    add_assoc_long(return_value, "stations_total", dwavdapi_group_statistics_stations_total(stats));
    
    state = dwavdapi_group_statistics_stations_state(stats);
    _dwavd_stats_array(&array, &state);
    add_assoc_zval(return_value, "stations_state", array);
    
    scans = dwavdapi_group_statistics_scans(stats);
    _dwavd_scans_array(&array, &scans);
    add_assoc_zval(return_value, "scans", array);
    
    infections = dwavdapi_group_statistics_infections(stats);
    _dwavd_infcd_array(&array, &infections);
    add_assoc_zval(return_value, "infections", array);
    
    list = dwavdapi_group_statistics_viruses_list(stats);
    _dwavd_res_list_to_array(&list_array TSRMLS_CC, list, le_dwavd_viruses_list);
    add_assoc_zval(return_value, "viruses", list_array);
}



/** ======= Tariff group info resource ======= */

/** mixed dwavd_trf_init()

   Initializes new empty tariff resource.
   Returns initialized resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_trf_init) {
    char *error = NULL;
    dwavdapi_group *grp = NULL;
    
    DWAVD_CHECK_PARAM_COUNT(0)
    
    grp = dwavdapi_group_init();
    if (NULL == grp) {
        dwavdapi_strerror(&error, errno);
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot initialize resource: %s (%d)", error, errno);
        if (error) {
            free(error);
            error = NULL;
        }
        RETURN_NULL();
    }    
    ZEND_REGISTER_RESOURCE(return_value, grp, le_dwavd_trf);
}


/** mixed dwavd_trf_get_info(resource connection_res)

   Makes a request to AV-Desk to get tariff information resource.
   Returns tariff information resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_trf_get_info) {
    zval *res = NULL;
    char *id = NULL;
    int id_len = 0;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_group *grp = NULL;
     
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &id, &id_len) == FAILURE) {
        return;
    }    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)    
    if(0 == id_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Tariff ID is not specified");
        RETURN_NULL();
    }            
    if (DWAVDAPI_FAILURE == dwavdapi_tariff_get_info(handle, &grp, id)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, grp, le_dwavd_trf);
}


/** bool dwavd_trf_free(resource trf_info_res)

   Frees memory allocated for a given tariff information resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_trf_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_group *grp = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res), le_dwavd_trf, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** mixed dwavd_trf(resource trf_info_res, string param_name)

   Returns a value of a given parameter from tariff resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_trf) {
    int rsrc_type = 0;
    int flag = -1;
    zval *res = NULL;
    zval *opt = NULL;
    zval *array = NULL;
    dwavdapi_group *grp = NULL;
    dwavdapi_list *list = NULL;
    dwavdapi_list *n_list = NULL;
    int rsrc_id = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res), le_dwavd_trf, rsrc_type)        
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_trfopt_array)
            
    switch(flag) {
        case DWAVD_TRF_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_group_id(grp));
        case DWAVD_TRF_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_group_name(grp));
        case DWAVD_TRF_PARENT_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_group_parent_id(grp));
        case DWAVD_TRF_DESCRIPTION:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_group_description(grp));
        case DWAVD_TRF_CREATED:
            RETURN_LONG(dwavdapi_group_created_time(grp));
        case DWAVD_TRF_MODIFIED:
            RETURN_LONG(dwavdapi_group_modified_time(grp));
        case DWAVD_TRF_TYPE:
            RETURN_LONG(dwavdapi_group_type(grp));
        case DWAVD_TRF_GRACE_PERIOD:
            RETURN_LONG(dwavdapi_group_grace_period(grp));
        case DWAVD_TRF_COMPONENTS:
            list = dwavdapi_group_components_list(grp);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_component_ctor);
            rsrc_id = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_components_list)
            RETURN_RESOURCE(rsrc_id);
         case DWAVD_TRF_RIGHTS:
            list = dwavdapi_group_rights_list(grp);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_right_ctor);
            rsrc_id = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_rights_list)
            RETURN_RESOURCE(rsrc_id);
        case DWAVD_TRF_CHILD_GROUPS_COUNT:
            RETURN_LONG(dwavdapi_group_child_groups_count(grp));
        case DWAVD_TRF_CHILD_GROUPS:
            _dwavd_carray_to_phparray(&array, (const char **)dwavdapi_group_child_groups_array(grp), dwavdapi_group_child_groups_count(grp));
            RETURN_ZVAL(array, 1, 0);        
        case DWAVD_TRF_STATIONS_COUNT:
            RETURN_LONG(dwavdapi_group_stations_count(grp));
        case DWAVD_TRF_STATIONS:
            _dwavd_carray_to_phparray(&array, (const char **)dwavdapi_group_stations_array(grp), dwavdapi_group_stations_count(grp));
            RETURN_ZVAL(array, 1, 0);        
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_trf_array(resource trf_info_res)

   Converts data from tariff information resource into an array and returns it.
   Returns an array with tariff resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_trf_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_list *list = NULL;
    dwavdapi_group *grp = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res), le_dwavd_trf, rsrc_type)
    array_init(return_value);
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "id", dwavdapi_group_id(grp)) 
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "name", dwavdapi_group_name(grp))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "description", dwavdapi_group_description(grp))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "parent_id", dwavdapi_group_parent_id(grp))
    add_assoc_long(return_value, "type", dwavdapi_group_type(grp));
    add_assoc_long(return_value, "created", dwavdapi_group_created_time(grp));
    add_assoc_long(return_value, "modified", dwavdapi_group_modified_time(grp));
    add_assoc_long(return_value, "grace_period", dwavdapi_group_grace_period(grp)); 
    
    list = dwavdapi_group_components_list(grp);
    _dwavd_res_list_to_array(&array TSRMLS_CC, list, le_dwavd_components_list);
    add_assoc_zval(return_value, "components", array);
    
    list = dwavdapi_group_rights_list(grp);
    _dwavd_res_list_to_array(&array TSRMLS_CC, list, le_dwavd_rights_list);
    add_assoc_zval(return_value, "rights", array);
    
    add_assoc_long(return_value, "child_groups_count", dwavdapi_group_child_groups_count(grp)); 
    _dwavd_carray_to_phparray(&array, (const char **)dwavdapi_group_child_groups_array(grp), dwavdapi_group_child_groups_count(grp));
    add_assoc_zval(return_value, "child_groups", array);    
    add_assoc_long(return_value, "stations_count", dwavdapi_group_stations_count(grp));
    _dwavd_carray_to_phparray(&array, (const char **)dwavdapi_group_stations_array(grp), dwavdapi_group_stations_count(grp));
    add_assoc_zval(return_value, "stations", array);
}


/* Helper function, used further down. */
static int _dwavd_trf_set(dwavdapi_group *grp, int flag TSRMLS_DC, zval *val) {    
    HashPosition pos = NULL;
    zval **array_item = NULL;
    char *array_strkey = NULL;
    unsigned int array_strkey_len = 0;
    unsigned long array_numkey = 0;
    int array_key_type = 0;

    switch(flag) {
        case DWAVD_TRF_ID:
           DWAVD_GRP_SET_STRING(grp, id, val, 0)
        case DWAVD_TRF_PARENT_ID:
           DWAVD_GRP_SET_STRING(grp, parent_id, val, 0)
        case DWAVD_TRF_NAME:
           DWAVD_GRP_SET_STRING(grp, name, val, 1)
        case DWAVD_TRF_DESCRIPTION:
           DWAVD_GRP_SET_STRING(grp, description, val, 0)
        case DWAVD_TRF_GRACE_PERIOD:
            DWAVD_GRP_SET_LONG(grp, grace_period, val)        
    }
    return 1;
}

/** bool dwavd_trf_set(resource trf_info_res, string param_name, mixed param_value)

   Sets a value of a given parameter to tariff information resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_trf_set) {
    int rsrc_type = 0;
    int flag = -1;
    zval *res = NULL;
    zval *opt = NULL;
    zval *val = NULL;
    dwavdapi_group *grp = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzz", &res, &opt, &val) == FAILURE) {
        return;
    }

    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res), le_dwavd_trf, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_trfopt_array)
            
    if(_dwavd_trf_set(grp, flag TSRMLS_CC, val)) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** bool dwavd_trf_set_array(resource trf_info_res, array params)

   Sets tariff information resource parameters according to data from a given params array.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_trf_set_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    HashPosition pos = NULL;
    zval *array = NULL;
    zval **array_item = NULL;
    char *array_strkey = NULL;
    unsigned int array_strkey_len = 0;
    unsigned long array_numkey = 0;
    int array_key_type = 0;
    dwavdapi_group *grp = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &array) == FAILURE) {
        return;
    }

    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res), le_dwavd_trf, rsrc_type)
    DWAVD_EXPECTED_ARRAY(array)
    DWAVD_ARRAY_EMPTY(Z_ARRVAL_P(array))
            
    for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos); 
       SUCCESS == zend_hash_has_more_elements_ex(Z_ARRVAL_P(array), &pos); zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos)) {
        array_key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(array), &array_strkey, &array_strkey_len, &array_numkey, 0, &pos);
        if(SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void**)&array_item, &pos)){
            if(array_key_type == HASH_KEY_IS_STRING) {
                flag = _dwavd_opt_to_flag(array_strkey, _dwavd_trfopt_array, sizeof(_dwavd_trfopt_array)/sizeof(_dwavd_trfopt_array[0]));
                if(-1 == flag) {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown option: `%s'", array_strkey);
                    RETURN_FALSE
                }
            } else {
                flag = array_numkey;
            }
            if(_dwavd_trf_set(grp, flag TSRMLS_CC, *array_item)) {
                RETURN_FALSE
            }
        }
    }    

    RETURN_TRUE
}


/** mixed dwavd_trf_add(resource connection_res, resource trf_res)

   Makes a request to AV-Desk to add tariff described by a given resource.
   Returns an ID for newly-created tariff, or false otherwise. */
PHP_FUNCTION(dwavd_trf_add) {
    zval *res = NULL;
    zval *res_grp = NULL;
    int rsrc_type = 0;
    int rsrc_grp_type = 0;
    char *id = NULL;
    dwavdapi_handle *handle = NULL;
    dwavdapi_group *grp = NULL;
   
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &res, &res_grp) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res_grp), le_dwavd_trf, rsrc_grp_type)

    if (DWAVDAPI_FAILURE == dwavdapi_tariff_add(handle, grp, &id)) {
        RETURN_FALSE
    }
    
    RETVAL_STRING(id, 1);
    free(id);
    return;
}


/** bool dwavd_trf_change(resource connection_res, resource trf_res)

   Makes a request to AV-Desk to update tariff with data from a given resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_trf_change) {
    zval *res = NULL;
    zval *res_grp = NULL;
    int rsrc_type = 0;
    int rsrc_grp_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_group *grp = NULL;
   
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &res, &res_grp) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res_grp), le_dwavd_trf, rsrc_grp_type)

    if (DWAVDAPI_FAILURE == dwavdapi_tariff_change(handle, grp)) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** bool dwavd_trf_delete(resource connection_res, string trf_id)

   Makes a request to AV-Desk to delete tariff with a given ID.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_trf_delete) {
    zval *res = NULL;
    int rsrc_type = 0;
    int id_len = 0;
    char *id = NULL;
    char *tid = NULL;
    int tid_len = 0;
    dwavdapi_handle *handle = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &res, &id, &id_len, &tid, &tid_len) == FAILURE) {
        return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)    
    if(0 == id_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Tariff ID is not specified");
        RETURN_NULL();
    }            
    if (DWAVDAPI_FAILURE == dwavdapi_tariff_delete(handle, id, tid)) {
        RETURN_FALSE
    }   
    
    RETURN_TRUE
}


/** bool dwavd_trf_set_component(resource trf_info_res, int component_id, int component_state_id)

   Sets tariff component information for a given tariff resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_trf_set_component) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_group *grp = NULL;
    long comp = 0;
    long val = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &res, &comp, &val) == FAILURE) {
        return;
    }
    DWAVD_FETCH_GRP_RES_WITH_RETURN_FALSE(grp, Z_RESVAL_P(res), le_dwavd_trf, rsrc_type)
    
    if(DWAVDAPI_FAILURE == dwavdapi_group_set_component(grp, comp, val)) {
        RETURN_FALSE
    }
    
    RETURN_TRUE
}



/** ======= Tariffs list resource ======= */

/** mixed dwavd_trf_get_list(resource connection_res)

   Makes a request to AV-Desk to retrieve a list of tariffs.
   Returns groups list resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_trf_get_list) {
    zval *res = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_list *list = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    
    if (DWAVDAPI_FAILURE == dwavdapi_tariff_get_list(handle, &list)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, list, le_dwavd_trf_list);    
}


/** bool dwavd_trf_list_free(resource trf_list_res)

   Frees memory allocated for a given tariffs list resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_trf_list_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_list *list = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_GRP_LST_RES_WITH_RETURN_FALSE(list, Z_RESVAL_P(res), le_dwavd_trf_list, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}



/** ======= List resource helpers ======= */


/** bool dwavd_list_next(resource list_res)

   Moves an internal pointer to next item in a given list.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_list_next) {
    zval *res = NULL;
    int res_found_type = 0;
    dwavdapi_list *lst = NULL;    
    unsigned int i = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_LST_RES_WITH_RETURN_FALSE(lst, Z_RESVAL_P(res), res_found_type)
    
    for(i = 0; i < _dwavd_lst_res_count; i++) {
        if(_dwavd_lst_res[i] == res_found_type) {
            if(DWAVDAPI_FAILURE == dwavdapi_list_next(lst)) {
                RETURN_FALSE
            }
            RETURN_TRUE
        }
    }

    DWAVD_INVALID_RESOURCE
    RETURN_FALSE
}


/** bool dwavd_list_prev(resource list_res)

   Moves an internal pointer to previous item in a given list.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_list_prev) {
    zval *res = NULL;
    int res_found_type = 0;
    dwavdapi_list *lst = NULL;    
    unsigned int i = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_LST_RES_WITH_RETURN_FALSE(lst, Z_RESVAL_P(res), res_found_type)
    
    for(i = 0; i < _dwavd_lst_res_count; i++) {
        if(_dwavd_lst_res[i] == res_found_type) {
            if(DWAVDAPI_FAILURE == dwavdapi_list_prev(lst)) {
                RETURN_FALSE
            }
            RETURN_TRUE
        }
    }
    DWAVD_INVALID_RESOURCE
    RETURN_FALSE
}


/** bool dwavd_list_rewind(resource list_res)

   Moves an internal pointer to the first item in a given list.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_list_rewind) {
    zval *res = NULL;
    int res_found_type = 0;
    dwavdapi_list *lst = NULL;    
    unsigned int i = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_LST_RES_WITH_RETURN_FALSE(lst, Z_RESVAL_P(res), res_found_type)
    
    for(i = 0; i < _dwavd_lst_res_count; i++) {
        if(_dwavd_lst_res[i] == res_found_type) {
            dwavdapi_list_reset(lst, DWAVDAPI_MOVE_INTERNAL_POINTER_TO_TOP);
            RETURN_TRUE
        }
    }
    DWAVD_INVALID_RESOURCE
    RETURN_FALSE
}


/** mixed dwavd_list_current(resource list_res)

   Returns current resource item in a given list, or NULL on fail. */
PHP_FUNCTION(dwavd_list_current) {
    zval *res = NULL;
    int res_found_type = -1;
    dwavdapi_list *lst = NULL;
    void *data = NULL;
    char *id = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_LST_RES_WITH_RETURN_NULL(lst, Z_RESVAL_P(res), res_found_type)
    DWAVD_LST_DATA(data, lst)
    
    if(le_dwavd_components_list == res_found_type) {
        ZEND_REGISTER_RESOURCE(return_value, (dwavdapi_component *)data, le_dwavd_component);
        return;
    } else if(le_dwavd_rights_list == res_found_type) {
        ZEND_REGISTER_RESOURCE(return_value, (dwavdapi_right *)data, le_dwavd_right);
        return;
    } else if(le_dwavd_bases_list == res_found_type) {
        ZEND_REGISTER_RESOURCE(return_value, (dwavdapi_base *)data, le_dwavd_base);
        return;
    } else if(le_dwavd_packages_list == res_found_type) {
        ZEND_REGISTER_RESOURCE(return_value, (dwavdapi_package *)data, le_dwavd_package);
        return;
    } else if(le_dwavd_modules_list == res_found_type) {
        ZEND_REGISTER_RESOURCE(return_value, (dwavdapi_module *)data, le_dwavd_module);
        return;
    } else if(le_dwavd_components_installed_list == res_found_type) {
        ZEND_REGISTER_RESOURCE(return_value, (dwavdapi_installed_component *)data, le_dwavd_component_installed);
        return;
    } else if(le_dwavd_components_running_list == res_found_type) {
        ZEND_REGISTER_RESOURCE(return_value, (dwavdapi_running_component *)data, le_dwavd_component_running);
        return;
    } else if(le_dwavd_trf_list == res_found_type || le_dwavd_grp_list == res_found_type) {
        id = ((dwavdapi_group *)data)->id;
        RETURN_STRING(id, 1);
        return;
    } else if(le_dwavd_adm_list == res_found_type) {
        id = ((dwavdapi_admin *)data)->login;
        RETURN_STRING(id, 1);
        return;
    } else if(le_dwavd_viruses_list == res_found_type) {
        ZEND_REGISTER_RESOURCE(return_value, (dwavdapi_virus *)data, le_dwavd_virus);
        return;
    } else if(le_dwavd_infcd_objs_list == res_found_type) {
        ZEND_REGISTER_RESOURCE(return_value, (dwavdapi_infected_object *)data, le_dwavd_infcd_obj);
        return;
    }
    
    DWAVD_INVALID_RESOURCE
    RETURN_NULL();
}


/* Helper function, used further down. */
static void _dwavd_res_list_to_array(zval **php_array TSRMLS_DC, dwavdapi_list *lst, int res_found_type) {
    void *data = NULL;
    zval *n_array = NULL;
    zval *h_array = NULL;
    unsigned int i = 0;
    char *id = NULL;
        
    MAKE_STD_ZVAL(n_array)
    array_init(n_array);
    
    if(NULL != lst) {
        if(le_dwavd_components_list == res_found_type) {
            do {
                data = dwavdapi_list_current_data(lst);
                if(NULL != data) {
                    _dwavd_component_array(&h_array, (dwavdapi_component *)data);
                    add_index_zval(n_array, i, h_array);
                    i++;
                }
            } while(dwavdapi_list_next(lst) != DWAVDAPI_FAILURE);
        } else if(le_dwavd_rights_list == res_found_type) {
            do {
                data = dwavdapi_list_current_data(lst);
                if(NULL != data) {
                    _dwavd_right_array(&h_array, (dwavdapi_right *)data);
                    add_index_zval(n_array, i, h_array);
                    i++;
                }
            } while(dwavdapi_list_next(lst) != DWAVDAPI_FAILURE);
        } else if(le_dwavd_components_installed_list == res_found_type) {
             do {
                data = dwavdapi_list_current_data(lst);
                if(NULL != data) {
                    _dwavd_component_installed_array(&h_array, (dwavdapi_installed_component *)data);
                    add_index_zval(n_array, i, h_array);
                    i++;
                }
            } while(dwavdapi_list_next(lst) != DWAVDAPI_FAILURE);
        } else if(le_dwavd_components_running_list == res_found_type) {
             do {
                data = dwavdapi_list_current_data(lst);
                if(NULL != data) {
                    _dwavd_component_running_array(&h_array, (dwavdapi_running_component *)data);
                    add_index_zval(n_array, i, h_array);
                    i++;
                }
            } while(dwavdapi_list_next(lst) != DWAVDAPI_FAILURE);
        } else if(le_dwavd_bases_list == res_found_type) {
             do {
                data = dwavdapi_list_current_data(lst);
                if(NULL != data) {
                    _dwavd_base_array(&h_array, (dwavdapi_base *)data);
                    add_index_zval(n_array, i, h_array);
                    i++;
                }
            } while(dwavdapi_list_next(lst) != DWAVDAPI_FAILURE);
        } else if(res_found_type==le_dwavd_packages_list) {
             do {
                data = dwavdapi_list_current_data(lst);
                if(data!=NULL) {
                    _dwavd_package_array(&h_array, (dwavdapi_package *)data);
                    add_index_zval(n_array, i, h_array);
                    i++;
                }
            } while (dwavdapi_list_next(lst)!=DWAVDAPI_FAILURE);
        } else if(le_dwavd_modules_list == res_found_type) {
             do {
                data = dwavdapi_list_current_data(lst);
                if(NULL != data) {
                    _dwavd_module_array(&h_array, (dwavdapi_module *)data);
                    add_index_zval(n_array, i, h_array);
                    i++;
                }
            } while(dwavdapi_list_next(lst) != DWAVDAPI_FAILURE);
        } else if(le_dwavd_trf_list == res_found_type || le_dwavd_grp_list == res_found_type) {
             do {
                data = dwavdapi_list_current_data(lst);
                if(NULL != data) {
                    id = ((dwavdapi_group *)data)->id;
                    add_index_string(n_array, i, id, 1);
                    i++;
                }
            } while(dwavdapi_list_next(lst) != DWAVDAPI_FAILURE);
        } else if(le_dwavd_adm_list == res_found_type) {
             do {
                data = dwavdapi_list_current_data(lst);
                if(NULL != data) {
                    id = ((dwavdapi_admin *)data)->id;
                    add_index_string(n_array, i, id, 1);
                    i++;
                }
            } while(dwavdapi_list_next(lst) != DWAVDAPI_FAILURE);
        } else if(le_dwavd_viruses_list == res_found_type) {
            do {
                data = dwavdapi_list_current_data(lst);
                if(NULL != data) {
                    _dwavd_virus_array(&h_array, (dwavdapi_virus *)data);
                    add_index_zval(n_array, i, h_array);
                    i++;
                }
            } while(dwavdapi_list_next(lst) != DWAVDAPI_FAILURE);
        } else if(le_dwavd_infcd_objs_list == res_found_type) {
            do {
                data = dwavdapi_list_current_data(lst);
                if(NULL != data) {
                    _dwavd_infcd_obj_array(&h_array, (dwavdapi_infected_object *)data);
                    add_index_zval(n_array, i, h_array);
                    i++;
                }
            } while(dwavdapi_list_next(lst) != DWAVDAPI_FAILURE);
        }
    }
    
    *php_array = n_array;
}


/** mixed dwavd_list_array(resource list_res)

   Converts a given resource into an array and returns it.
   Returns an array on success, or false otherwise. */
PHP_FUNCTION(dwavd_list_array) {
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_list *lst = NULL;
    int res_found_type = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_LST_RES_WITH_RETURN_FALSE(lst, Z_RESVAL_P(res), res_found_type)
            
    _dwavd_res_list_to_array(&array TSRMLS_CC, lst, res_found_type);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}



/** ======= AV Component resource ======= */

/** mixed dwavd_component(resource compon_res, string param_name)

   Returns a value of a given parameter from available component resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_component) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_component *cmp = NULL;
    zval *opt = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_CMPS_RES_WITH_RETURN_FALSE(cmp, Z_RESVAL_P(res), le_dwavd_component, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_cmpopt_array)
    switch(flag) {
        case DWAVD_CMP_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_component_name(cmp));
        case DWAVD_CMP_INHERITED_GROUP_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_component_inherited_group_id(cmp));
        case DWAVD_CMP_CODE:
            RETURN_LONG(dwavdapi_component_code(cmp));
        case DWAVD_CMP_VALUE:
            RETURN_LONG(dwavdapi_component_value(cmp));
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_component_installed(resource icompon_res, string param_name)

   Returns a value of a given parameter from installed on station component resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_component_installed) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_installed_component *cmp = NULL;
    zval *opt = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_CMP_INSTD_RES_WITH_RETURN_FALSE(cmp, Z_RESVAL_P(res), le_dwavd_component_installed, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_cmp_instd_opt_array)
    switch(flag) {
        case DWAVD_CMP_INSTD_PATH:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_component_installed_path(cmp));
        case DWAVD_CMP_INSTD_SERVER:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_component_installed_server(cmp));
        case DWAVD_CMP_INSTD_INSTALLED:
            RETURN_LONG(dwavdapi_component_installed_time(cmp));
        case DWAVD_CMP_INSTD_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_component_installed_name(cmp));
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_component_installed(resource rcompon_res, string param_name)

   Returns a value of a given parameter from running on station component resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_component_running) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_running_component *cmp = NULL;
    zval *opt = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_CMP_RUN_RES_WITH_RETURN_FALSE(cmp, Z_RESVAL_P(res), le_dwavd_component_running, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_cmp_run_opt_array)
    switch(flag) {
        case DWAVD_CMP_RUN_PARAMS:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_component_running_params(cmp));
        case DWAVD_CMP_RUN_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_component_running_name(cmp));
        case DWAVD_CMP_RUN_USER:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_component_running_user(cmp));
        case DWAVD_CMP_RUN_STARTED:
            RETURN_LONG(dwavdapi_component_running_started_time(cmp));
        case DWAVD_CMP_RUN_TYPE:
            RETURN_LONG(dwavdapi_component_running_type(cmp));
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_component_array(resource compon_res)

   Converts data from available component resource into an array and returns it.
   Returns an array with available component resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_component_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_component *cmp = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_CMPS_RES_WITH_RETURN_FALSE(cmp, Z_RESVAL_P(res), le_dwavd_component, rsrc_type)
    _dwavd_component_array(&array, cmp);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}


/** mixed dwavd_component_installed_array(resource icompon_res)

   Converts data from installed on station component resource into an array and returns it.
   Returns an array with installed on station component resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_component_installed_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_installed_component *cmp = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_CMP_INSTD_RES_WITH_RETURN_FALSE(cmp, Z_RESVAL_P(res), le_dwavd_component_installed, rsrc_type)
    _dwavd_component_installed_array(&array, cmp);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}


/** mixed dwavd_component_running_array(resource rcompon_res)

   Converts data from running on station component resource into an array and returns it.
   Returns an array with running on station component resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_component_running_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_running_component *cmp = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_CMP_RUN_RES_WITH_RETURN_FALSE(cmp, Z_RESVAL_P(res), le_dwavd_component_running, rsrc_type)
    _dwavd_component_running_array(&array, cmp);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}



/** ======= Station rights resource ======= */

/** mixed dwavd_right(resource right_res, string param_name)

   Returns a value of a given parameter from right resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_right) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_right *rht = NULL;
    zval *opt = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_RHT_RES_WITH_RETURN_FALSE(rht, Z_RESVAL_P(res), le_dwavd_right, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_rhtopt_array)
    switch(flag) {
        case DWAVD_RHT_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_right_name(rht));
        case DWAVD_RHT_INHERITED_GROUP_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_right_inherited_group_id(rht));
        case DWAVD_RHT_CODE:
            RETURN_LONG(dwavdapi_right_code(rht));
        case DWAVD_RHT_VALUE:
            RETURN_LONG(dwavdapi_right_value(rht));
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_right_array(resource right_res)

   Converts data from right resource into an array and returns it.
   Returns an array with right resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_right_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_right *rht = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_RHT_RES_WITH_RETURN_FALSE(rht, Z_RESVAL_P(res), le_dwavd_right, rsrc_type)
    array_init(return_value);
    _dwavd_right_array(&array, rht);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}



/** ======= AV Base resource ======= */

/** mixed dwavd_base(resource base_res, string param_name)

   Returns a value of a given parameter from AV base resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_base) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_base *base = NULL;
    zval *opt = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_BASE_RES_WITH_RETURN_FALSE(base, Z_RESVAL_P(res), le_dwavd_base, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_baseopt_array)
    switch(flag) {
        case DWAVD_BASE_FILE:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_base_file_name(base));
        case DWAVD_BASE_VERSION:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_base_version(base));
        case DWAVD_BASE_CREATED:
            RETURN_LONG(dwavdapi_base_created_time(base));
        case DWAVD_BASE_VIRUSES:
            RETURN_LONG(dwavdapi_base_viruses(base));

    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_base_array(resource base_res)

   Converts data from AV base resource into an array and returns it.
   Returns an array with AV base resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_base_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_base *base = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_BASE_RES_WITH_RETURN_FALSE(base, Z_RESVAL_P(res), le_dwavd_base, rsrc_type)
    array_init(return_value);
    _dwavd_base_array(&array, base);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}



/** ======= AV-Agent package resource ======= */

/** mixed dwavd_package(resource package_res, string param_name)

   Returns a value of a given parameter from AV-agent package resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_package) {
    int res_type = 0;
    int flag = -1;
    zval *opt = NULL;
    zval *res = NULL;
    dwavdapi_package *package = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt)==FAILURE) {
        return;
    }
    DWAVD_FETCH_PACKAGE_RES_WITH_RETURN_FALSE(package, Z_RESVAL_P(res), le_dwavd_package, res_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_packopt_array)
    switch(flag) {
        case DWAVD_PACKAGE_URL:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_package_url(package));
        case DWAVD_PACKAGE_TYPE:
            RETURN_LONG(dwavdapi_package_type(package));

    }
    DWAVD_UNKNOWN_OPTION(opt)
}

/** mixed dwavd_package_array(resource package_res)

   Converts data from AV-agent package resource into an array and returns it.
   Returns an array with AV-agent package resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_package_array) {
    int res_type = 0;
    zval *array = NULL;
    zval *res = NULL;
    dwavdapi_package *package = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res)==FAILURE) {
        return;
    }
    DWAVD_FETCH_PACKAGE_RES_WITH_RETURN_FALSE(package, Z_RESVAL_P(res), le_dwavd_package, res_type)
    array_init(return_value);
    _dwavd_package_array(&array, package);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}



/** ======= AV Module resource ======= */

/* Helper function, used further down. */
static void _dwavd_module_array(zval **php_array, const dwavdapi_module *module) {
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    if(NULL != module) {
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "file", (char *)dwavdapi_module_file_name(module)) 
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "hash", (char *)dwavdapi_module_hash(module))
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "name", (char *)dwavdapi_module_name(module))
        DWAVD_ADD_ASSOC_STRING_OR_NULL(array, "version", (char *)dwavdapi_module_version(module))
        add_assoc_long(array, "modified", dwavdapi_module_modified_time(module));
        add_assoc_long(array, "created", dwavdapi_module_created_time(module)); 
        add_assoc_double(array, "size", dwavdapi_module_file_size(module));
    }
    *php_array = array;
}


/** mixed dwavd_module(resource module_res, string param_name)

   Returns a value of a given parameter from AV module resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_module) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_module *module = NULL;
    zval *opt = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_MOD_RES_WITH_RETURN_FALSE(module, Z_RESVAL_P(res), le_dwavd_module, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_moduleopt_array)
    switch(flag) {
        case DWAVD_MOD_FILE:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_module_file_name(module));
        case DWAVD_MOD_HASH:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_module_hash(module));
        case DWAVD_MOD_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_module_name(module));
        case DWAVD_MOD_VERSION:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_module_version(module));
        case DWAVD_MOD_CREATED:
            RETURN_LONG(dwavdapi_module_created_time(module));
        case DWAVD_MOD_MODIFIED:
            RETURN_LONG(dwavdapi_module_modified_time(module));
        case DWAVD_MOD_SIZE:
            RETURN_DOUBLE(dwavdapi_module_file_size(module));
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_module_array(resource module_res)

   Converts data from AV module resource into an array and returns it.
   Returns an array with AV module resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_module_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_module *module = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_MOD_RES_WITH_RETURN_FALSE(module, Z_RESVAL_P(res), le_dwavd_module, rsrc_type)
    array_init(return_value);
    _dwavd_module_array(&array, module);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}



/** ======= Station info resource ======= */

/** mixed dwavd_st_init()

   Initializes new empty station resource.
   Returns initialized resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_st_init) {
    char *error = NULL;
    dwavdapi_station *st = NULL;
    
    DWAVD_CHECK_PARAM_COUNT(0)
    
    st = dwavdapi_station_init();
    if (NULL == st) {
        dwavdapi_strerror(&error, errno);
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot initialize resource: %s (%d)", error, errno);
        if (error) {
            free(error);
            error = NULL;
        }
        RETURN_NULL();
    }    
    ZEND_REGISTER_RESOURCE(return_value, st, le_dwavd_st);
}


/** mixed dwavd_st_get_info(resource connection_res)

   Makes a request to AV-Desk to get station information resource.
   Returns station information resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_st_get_info) {
    zval *res = NULL;
    char *id = NULL;
    int id_len = 0;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_station *st = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &id, &id_len) == FAILURE) {
        return;
    }    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)    
    if(0 == id_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Station ID is not specified");
        RETURN_NULL();
    }            
    if (DWAVDAPI_FAILURE == dwavdapi_station_get_info(handle, &st, id)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, st, le_dwavd_st);
}


/** bool dwavd_st_free(resource st_info_res)

   Frees memory allocated for a given station information resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_st_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_station *st = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_ST_RES_WITH_RETURN_FALSE(st, Z_RESVAL_P(res), le_dwavd_st, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** mixed dwavd_st(resource st_info_res, string param_name)

   Returns a value of a given parameter from station resource.
   On failure returns false. */
DIAGNOSTIC_OFF(deprecated-declarations)
PHP_FUNCTION(dwavd_st) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_station *st = NULL;
    zval *opt = NULL;
    int flag = -1;
    zval *emails = NULL;
    zval *groups = NULL;
    dwavdapi_list *list = NULL;
    dwavdapi_list *n_list = NULL;
    int rsrc_id = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_ST_RES_WITH_RETURN_FALSE(st, Z_RESVAL_P(res), le_dwavd_st, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_stopt_array)
   
    switch(flag) {
        case DWAVD_ST_OS:
            RETURN_LONG(dwavdapi_station_os(st))
        case DWAVD_ST_OS_NAME:
            DWAVD_RETURN_STRING_OR_NULL((char *)dwavdapi_station_os_str(st))
        case DWAVD_ST_STATE:
            RETURN_LONG(dwavdapi_station_state(st))
        case DWAVD_ST_COUNTRY:
            RETURN_LONG(dwavdapi_station_country(st))
        case DWAVD_ST_COUNTRY_NAME:
            DWAVD_RETURN_STRING_OR_NULL((char *)dwavdapi_country_fullname(dwavdapi_station_country(st)))
        case DWAVD_ST_COUNTRY_CODE:
             DWAVD_RETURN_STRING_OR_NULL((char *)dwavdapi_country_shortname(dwavdapi_station_country(st)))
        case DWAVD_ST_GRACE_PERIOD:
            RETURN_LONG(dwavdapi_station_grace_period(st))
        case DWAVD_ST_LONGITUDE:
            RETURN_LONG(dwavdapi_station_longitude(st))
        case DWAVD_ST_LATITUDE:
            RETURN_LONG(dwavdapi_station_latitude(st))
        case DWAVD_ST_CREATED:
            RETURN_LONG(dwavdapi_station_created_time(st))
        case DWAVD_ST_MODIFIED:
            RETURN_LONG(dwavdapi_station_modified_time(st))
        case DWAVD_ST_EXPIRES:
	    if(dwavdapi_station_expires_time(st)==DWAVDAPI_NOT_INITIALIZED || 
               dwavdapi_station_expires_time(st)==DWAVDAPI_RESET) {
		RETURN_NULL()
	    } 
    	    RETURN_LONG(dwavdapi_station_expires_time(st))
        case DWAVD_ST_CITY:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_city(st))
        case DWAVD_ST_FLOOR:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_floor(st))
        case DWAVD_ST_CONFIG:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_config(st))
        case DWAVD_ST_URL:            
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_url(st))                 
        case DWAVD_ST_PASSWORD:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_password(st))
        case DWAVD_ST_DESCRIPTION:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_description(st))
        case DWAVD_ST_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_id(st))
        case DWAVD_ST_ROOM:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_room(st))
        case DWAVD_ST_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_name(st))
        case DWAVD_ST_STREET:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_street(st))
        case DWAVD_ST_PROVINCE:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_province(st))
        case DWAVD_ST_DEPARTMENT:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_department(st))
        case DWAVD_ST_TARIFF_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_tariff_id(st))
        case DWAVD_ST_ORGANIZATION:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_organization(st))
        case DWAVD_ST_PARENT_ID: 
        case DWAVD_ST_EMAILS:
            _dwavd_carray_to_phparray(&emails, (const char **)dwavdapi_station_emails_array(st), dwavdapi_station_emails_count(st));
            RETURN_ZVAL(emails, 1, 0);
        case DWAVD_ST_EMAILS_COUNT:
            RETURN_LONG(dwavdapi_station_emails_count(st))                
        case DWAVD_ST_GROUPS:
            _dwavd_carray_to_phparray(&groups, (const char **)dwavdapi_station_groups_array(st), dwavdapi_station_groups_count(st));
            RETURN_ZVAL(groups, 1, 0);
        case DWAVD_ST_GROUPS_COUNT:
            RETURN_LONG(dwavdapi_station_groups_count(st))
        case DWAVD_ST_COMPONENTS:
            list = dwavdapi_station_components_list(st);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_component_ctor);
            rsrc_id = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_components_list)
            RETURN_RESOURCE(rsrc_id);     
        case DWAVD_ST_BASES:
            list = dwavdapi_station_bases_list(st);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_base_ctor);
            rsrc_id = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_bases_list)
            RETURN_RESOURCE(rsrc_id);     
        case DWAVD_ST_PACKAGES:
            list = dwavdapi_station_packages_list(st);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_package_ctor);
            rsrc_id = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_packages_list)
            RETURN_RESOURCE(rsrc_id);     
        case DWAVD_ST_MODULES:
            list = dwavdapi_station_modules_list(st);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_module_ctor);
            rsrc_id = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_modules_list)
            RETURN_RESOURCE(rsrc_id);  
        case DWAVD_ST_RIGHTS:
            list = dwavdapi_station_rights_list(st);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_right_ctor);
            rsrc_id = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_rights_list)
            RETURN_RESOURCE(rsrc_id);  
        case DWAVD_ST_COMPONENTS_RUN:
            list = dwavdapi_station_components_running_list(st);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_component_running_ctor);
            rsrc_id = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_component_running)
            RETURN_RESOURCE(rsrc_id);  
        case DWAVD_ST_COMPONENTS_INSTD:
            list = dwavdapi_station_components_installed_list(st);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_component_installed_ctor);
            rsrc_id = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_component_installed)
            RETURN_RESOURCE(rsrc_id); 
        case DWAVD_ST_BLOCK_BEGINS:
    	    if(dwavdapi_station_block_time_begin(st)==DWAVDAPI_NOT_INITIALIZED || 
               dwavdapi_station_block_time_begin(st)==DWAVDAPI_RESET) {
		RETURN_NULL()
	    } 
            RETURN_LONG(dwavdapi_station_block_time_begin(st))
        case DWAVD_ST_BLOCK_ENDS:
            if(dwavdapi_station_block_time_end(st)==DWAVDAPI_NOT_INITIALIZED || 
               dwavdapi_station_block_time_end(st)==DWAVDAPI_RESET) {
		RETURN_NULL()
	    }
            RETURN_LONG(dwavdapi_station_block_time_end(st))
        case DWAVD_ST_LASTSEEN_TIME:
            RETURN_LONG(dwavdapi_station_lastseen_time(st))
        case DWAVD_ST_LASTSEEN_ADDR:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_station_lastseen_addr(st))
    }
    DWAVD_UNKNOWN_OPTION(opt)
}
DIAGNOSTIC_ON(deprecated-declarations)

/** mixed dwavd_st_array(resource st_info_res)

   Converts data from station information resource into an array and returns it.
   Returns an array with station resource data on success, or false otherwise. */
DIAGNOSTIC_OFF(deprecated-declarations)
PHP_FUNCTION(dwavd_st_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_station *st = NULL;    
    zval *array = NULL;
    dwavdapi_list *list = NULL;
        
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_ST_RES_WITH_RETURN_FALSE(st, Z_RESVAL_P(res), le_dwavd_st, rsrc_type)
    array_init(return_value);
    
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "id", dwavdapi_station_id(st)) 
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "name", dwavdapi_station_name(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "password", dwavdapi_station_password(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "description", dwavdapi_station_description(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "parent_id", dwavdapi_station_parent_id(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "tariff_id", dwavdapi_station_tariff_id(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "country_name", (char *)dwavdapi_country_fullname(dwavdapi_station_country(st)))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "country_code", (char *)dwavdapi_country_shortname(dwavdapi_station_country(st)))    
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "city", dwavdapi_station_city(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "room", dwavdapi_station_room(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "floor", dwavdapi_station_floor(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "department", dwavdapi_station_department(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "province", dwavdapi_station_province(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "config", dwavdapi_station_config(st))            
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "url", dwavdapi_station_url(st))          
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "organization", dwavdapi_station_organization(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "lastseen_addr", dwavdapi_station_lastseen_addr(st))
    DWAVD_ADD_ASSOC_STRING_OR_NULL(return_value, "os_name", (char *)dwavdapi_station_os_str(st))
    add_assoc_long(return_value, "lastseen_time", dwavdapi_station_lastseen_time(st));
    add_assoc_long(return_value, "country", dwavdapi_station_country(st));
    add_assoc_long(return_value, "os", dwavdapi_station_os(st));
    add_assoc_long(return_value, "created", dwavdapi_station_created_time(st));
    add_assoc_long(return_value, "modified", dwavdapi_station_modified_time(st));
    
    if (dwavdapi_station_expires_time(st)==DWAVDAPI_NOT_INITIALIZED || 
       dwavdapi_station_expires_time(st)==DWAVDAPI_RESET) {
	add_assoc_null(return_value, "expires");
    } else {
	add_assoc_long(return_value, "expires", dwavdapi_station_expires_time(st));
    }
    
    add_assoc_long(return_value, "grace_period", dwavdapi_station_grace_period(st));
    add_assoc_long(return_value, "state", dwavdapi_station_state(st));
    add_assoc_long(return_value, "longitude", dwavdapi_station_longitude(st));
    add_assoc_long(return_value, "latitude", dwavdapi_station_latitude(st));    
    add_assoc_long(return_value, "state", dwavdapi_station_state(st));
    
    if (dwavdapi_station_block_time_begin(st)==DWAVDAPI_NOT_INITIALIZED ||
        dwavdapi_station_block_time_begin(st)==DWAVDAPI_RESET) {
	add_assoc_null(return_value, "block_begins");
    } else {
	add_assoc_long(return_value, "block_begins",  dwavdapi_station_block_time_begin(st));
    }
    
    if (dwavdapi_station_block_time_end(st)==DWAVDAPI_NOT_INITIALIZED ||
        dwavdapi_station_block_time_end(st)==DWAVDAPI_RESET) {
	add_assoc_null(return_value, "block_ends");
    } else {
	add_assoc_long(return_value, "block_ends", dwavdapi_station_block_time_end(st));
    }
    
    add_assoc_long(return_value, "groups_count", dwavdapi_station_groups_count(st));    
    _dwavd_carray_to_phparray(&array, (const char **)dwavdapi_station_groups_array(st), dwavdapi_station_groups_count(st));
    add_assoc_zval(return_value, "groups", array);
    
    add_assoc_long(return_value, "emails_count", dwavdapi_station_emails_count(st));
    _dwavd_carray_to_phparray(&array, (const char **)dwavdapi_station_emails_array(st), dwavdapi_station_emails_count(st));
    add_assoc_zval(return_value, "emails", array); 
    
    list = dwavdapi_station_rights_list(st);
    _dwavd_res_list_to_array(&array TSRMLS_CC, list, le_dwavd_rights_list);
    add_assoc_zval(return_value, "rights", array);
    
    list = dwavdapi_station_components_list(st);
    _dwavd_res_list_to_array(&array TSRMLS_CC, list, le_dwavd_components_list);
    add_assoc_zval(return_value, "components", array);
        
    list = dwavdapi_station_components_installed_list(st);
    _dwavd_res_list_to_array(&array TSRMLS_CC, list, le_dwavd_components_installed_list);
    add_assoc_zval(return_value, "components_installed", array);
   
   list = dwavdapi_station_components_running_list(st);
   _dwavd_res_list_to_array(&array TSRMLS_CC, list, le_dwavd_components_running_list);
   add_assoc_zval(return_value, "components_running", array);
    
    list = dwavdapi_station_bases_list(st);
    _dwavd_res_list_to_array(&array TSRMLS_CC, list, le_dwavd_bases_list);
    add_assoc_zval(return_value, "bases", array);
    
    list = dwavdapi_station_packages_list(st);
    _dwavd_res_list_to_array(&array TSRMLS_CC, list, le_dwavd_packages_list);
    add_assoc_zval(return_value, "packages", array);
    
    list = dwavdapi_station_modules_list(st);
    _dwavd_res_list_to_array(&array TSRMLS_CC, list, le_dwavd_modules_list);
    add_assoc_zval(return_value, "modules", array);
}
DIAGNOSTIC_ON(deprecated-declarations)

/* Helper function, used further down. */
DIAGNOSTIC_OFF(deprecated-declarations)
static int _dwavd_st_set(dwavdapi_station *st, int flag TSRMLS_DC, zval *val) {    
    HashPosition pos = NULL;
    zval **array_item = NULL;
    char *array_strkey = NULL;
    unsigned int array_strkey_len = 0;
    unsigned long array_numkey = 0;
    int array_key_type = 0;
    int country_id = -1;

    switch(flag) {
        case DWAVD_ST_COUNTRY:
            country_id = dwavdapi_country_id_by_code(Z_STRVAL_P(val));
            if(country_id > 0) {
                if(DWAVDAPI_FAILURE == dwavdapi_station_set_country(st, country_id)) { \
                   return 1;
                }
            } else {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Incorrect country code");
                return 1;
            }
            return 0;
        case DWAVD_ST_LONGITUDE:
            DWAVD_ST_SET_LONG(st, longitude, val)
        case DWAVD_ST_LATITUDE:
            DWAVD_ST_SET_LONG(st, latitude, val)
        case DWAVD_ST_EXPIRES:
            if(Z_TYPE_P(val) == IS_NULL) { 
                if(DWAVDAPI_SUCCESS == dwavdapi_station_set_expires_time(st, DWAVDAPI_RESET)) { 
                    return 0;
                } 
             } else if(Z_TYPE_P(val) == IS_LONG) { 
                if(DWAVDAPI_SUCCESS == dwavdapi_station_set_expires_time(st, Z_LVAL_P(val))) { 
                    return 0; 
                } 
             } else {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected integer, got %s", _dwavd_var_type(val)); 
                return 1;
             }
            break;
        case DWAVD_ST_CITY:
             DWAVD_ST_SET_STRING(st, city, val, 0)
        case DWAVD_ST_FLOOR:
            DWAVD_ST_SET_STRING(st, floor, val, 0)       
        case DWAVD_ST_PASSWORD:
            DWAVD_ST_SET_STRING(st, password, val, 0)
        case DWAVD_ST_DESCRIPTION:
            DWAVD_ST_SET_STRING(st, description, val, 0)
        case DWAVD_ST_ID:
            DWAVD_ST_SET_STRING(st, id, val, 0)
        case DWAVD_ST_ROOM:
            DWAVD_ST_SET_STRING(st, room, val, 0)
        case DWAVD_ST_NAME:
            DWAVD_ST_SET_STRING(st, name, val, 0)
        case DWAVD_ST_STREET:
            DWAVD_ST_SET_STRING(st, street, val, 0)
        case DWAVD_ST_PROVINCE:
            DWAVD_ST_SET_STRING(st, province, val, 0)
        case DWAVD_ST_DEPARTMENT:
            DWAVD_ST_SET_STRING(st, department, val, 0)
        case DWAVD_ST_TARIFF_ID:
            DWAVD_ST_SET_STRING(st, tariff_id, val, 0)
        case DWAVD_ST_ORGANIZATION:
            DWAVD_ST_SET_STRING(st, organization, val, 0)
        case DWAVD_ST_PARENT_ID:
            DWAVD_ST_SET_STRING(st, parent_id, val, 0)
        case DWAVD_ST_EMAILS:
            DWAVD_EXPECTED_ARRAY_WITH_RET(val, 1)
            for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(val), &pos); 
               SUCCESS == zend_hash_has_more_elements_ex(Z_ARRVAL_P(val), &pos); zend_hash_move_forward_ex(Z_ARRVAL_P(val), &pos)) {
                array_key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(val), &array_strkey, &array_strkey_len, &array_numkey, 0, &pos);
                if(HASH_KEY_IS_LONG != array_key_type) {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key of array must be integer, got `%s'", _dwavd_var_type(*array_item));
                    return 1;
                }
                if(SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(val), (void**)&array_item, &pos)){
                    if(Z_TYPE_PP(array_item) != IS_STRING) {
                        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Value of array must be string, got `%s'", _dwavd_var_type(*array_item));
                        return 1;
                    }                    
                    if(DWAVDAPI_FAILURE == dwavdapi_station_add_email(st, Z_STRVAL_PP(array_item))) {
                        return 1;
                    }                  
                }
            }
            return 0;
        case DWAVD_ST_DEL_EMAILS:
            DWAVD_EXPECTED_ARRAY_WITH_RET(val, 1)
            for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(val), &pos); 
               SUCCESS == zend_hash_has_more_elements_ex(Z_ARRVAL_P(val), &pos); zend_hash_move_forward_ex(Z_ARRVAL_P(val), &pos)) {
                array_key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(val), &array_strkey, &array_strkey_len, &array_numkey, 0, &pos);
                if(HASH_KEY_IS_LONG != array_key_type) {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key of array must be integer, got `%s'", _dwavd_var_type(*array_item));
                    return 1;
                }
                if(SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(val), (void**)&array_item, &pos)){
                    if(Z_TYPE_PP(array_item) != IS_STRING) {
                        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Value of array must be string, got `%s'", _dwavd_var_type(*array_item));
                        return 1;
                    }                    
                    if(DWAVDAPI_FAILURE == dwavdapi_station_delete_email(st, Z_STRVAL_PP(array_item))) {
                        return 1;
                    }                  
                }
            }
            return 0;
        case DWAVD_ST_GROUPS:
            DWAVD_EXPECTED_ARRAY_WITH_RET(val, 1)
            for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(val), &pos); 
               SUCCESS == zend_hash_has_more_elements_ex(Z_ARRVAL_P(val), &pos); zend_hash_move_forward_ex(Z_ARRVAL_P(val), &pos)) {
                array_key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(val), &array_strkey, &array_strkey_len, &array_numkey, 0, &pos);
                if(HASH_KEY_IS_LONG != array_key_type) {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Key of array must be integer, got `%s'", _dwavd_var_type(*array_item));
                    return 1;
                }
                if(SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(val), (void**)&array_item, &pos)){
                    if(Z_TYPE_PP(array_item) != IS_STRING) {
                        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Value of array must be string, got `%s'", _dwavd_var_type(*array_item));
                        return 1;
                    }
                    if(DWAVDAPI_FAILURE == dwavdapi_station_add_to_group(st, Z_STRVAL_PP(array_item))) {
                        return 1;
                    }
                }
            }
            return 0;
        case DWAVD_ST_BLOCK_BEGINS:
             if(Z_TYPE_P(val) == IS_NULL) { 
                if(DWAVDAPI_SUCCESS == dwavdapi_station_set_block_time(st, DWAVDAPI_RESET, -1)) { 
                    return 0;
                } 
             } else if(Z_TYPE_P(val) == IS_LONG) { 
                if(DWAVDAPI_SUCCESS == dwavdapi_station_set_block_time(st, Z_LVAL_P(val), -1)) { 
                    return 0; 
                } 
             } else {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected integer, got %s", _dwavd_var_type(val)); 
                return 1; 
             }
             break;
        case DWAVD_ST_BLOCK_ENDS:
            if(Z_TYPE_P(val) == IS_NULL) { 
                if(DWAVDAPI_SUCCESS == dwavdapi_station_set_block_time(st, -1, DWAVDAPI_RESET)) { 
                    return 0;
                } 
            } else if(Z_TYPE_P(val) == IS_LONG) { 
                if(DWAVDAPI_SUCCESS == dwavdapi_station_set_block_time(st, -1, Z_LVAL_P(val))) { 
                    return 0; 
                } 
            } else {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected integer, got %s", _dwavd_var_type(val)); 
                return 1; 
            }
            break;
        case DWAVD_ST_SCC:
    	    if(Z_TYPE_P(val) == IS_BOOL) {
    		if(DWAVDAPI_SUCCESS == dwavdapi_station_set_scc(st, Z_BVAL_P(val))) { 
                    return 0; 
                } 
    	    } else {
    		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected bool, got %s", _dwavd_var_type(val)); 
                return 1; 
    	    }
            break;
    }    
    return 1;
}
DIAGNOSTIC_ON(deprecated-declarations)

/** bool dwavd_st_set(resource st_info_res, string param_name, mixed param_value)

   Sets a value of a given parameter to station information resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_st_set) {
    int rsrc_type = 0;
    zval *res = NULL;
    int flag = -1;
    dwavdapi_station *st = NULL;
    zval *opt = NULL;
    zval *val = NULL;    
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzz", &res, &opt, &val) == FAILURE) {
        return;
    }
    DWAVD_FETCH_ST_RES_WITH_RETURN_FALSE(st, Z_RESVAL_P(res), le_dwavd_st, rsrc_type)        
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_stopt_array)
    
    if(_dwavd_st_set(st, flag TSRMLS_CC, val)) {
        RETURN_FALSE
    }
    
    RETURN_TRUE
}


/** bool dwavd_st_set_array(resource st_info_res, array params)

   Sets station information resource parameters according to data from a given params array.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_st_set_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    HashPosition pos = NULL;
    zval *array = NULL;
    zval **array_item = NULL;
    char *array_strkey = NULL;
    unsigned int array_strkey_len = 0;
    unsigned long array_numkey = 0;
    int array_key_type = 0;
    dwavdapi_station *st = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &array) == FAILURE) {
        return;
    }

    DWAVD_FETCH_ST_RES_WITH_RETURN_FALSE(st, Z_RESVAL_P(res), le_dwavd_st, rsrc_type)
    DWAVD_EXPECTED_ARRAY(array)
    DWAVD_ARRAY_EMPTY(Z_ARRVAL_P(array))
            
    for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos); 
       SUCCESS == zend_hash_has_more_elements_ex(Z_ARRVAL_P(array), &pos); zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos)) {
        array_key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(array), &array_strkey, &array_strkey_len, &array_numkey, 0, &pos);
        if(SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void**)&array_item, &pos)){
            if(array_key_type == HASH_KEY_IS_STRING) {
                flag = _dwavd_opt_to_flag(array_strkey, _dwavd_stopt_array, sizeof(_dwavd_stopt_array)/sizeof(_dwavd_stopt_array[0]));
                if(-1 == flag) {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown option: `%s'", array_strkey);
                    RETURN_FALSE
                }
            } else {
                flag = array_numkey;
            }
            if(_dwavd_st_set(st, flag TSRMLS_CC, *array_item)) {
                RETURN_FALSE
            }
        }
    }    

    RETURN_TRUE
}


/** mixed dwavd_st_add(resource connection_res, resource st_res)

   Makes a request to AV-Desk to add station described by a given resource.
   Returns an ID for newly-created station, or false otherwise. */
PHP_FUNCTION(dwavd_st_add) {
    zval *res = NULL;
    zval *res_st = NULL;
    int rsrc_type = 0;
    int rsrc_st_type = 0;
    char *id = NULL;
    dwavdapi_handle *handle = NULL;
    dwavdapi_station *st = NULL;
   
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &res, &res_st) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    DWAVD_FETCH_ST_RES_WITH_RETURN_FALSE(st, Z_RESVAL_P(res_st), le_dwavd_st, rsrc_st_type)

    if (DWAVDAPI_FAILURE == dwavdapi_station_add(handle, st, &id)) {
        RETURN_FALSE
    }
    
    RETVAL_STRING(id, 1);
    free(id);
    return;
}


/** bool dwavd_st_change(resource connection_res, resource st_res)

   Makes a request to AV-Desk to update station with data from a given resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_st_change) {
    zval *res = NULL;
    zval *res_st = NULL;
    int rsrc_type = 0;
    int rsrc_st_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_station *st = NULL;
   
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &res, &res_st) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
    DWAVD_FETCH_ST_RES_WITH_RETURN_FALSE(st, Z_RESVAL_P(res_st), le_dwavd_st, rsrc_st_type)

    if (DWAVDAPI_FAILURE == dwavdapi_station_change(handle, st)) {
        RETURN_FALSE
    }
    
    RETURN_TRUE
}


/** bool dwavd_st_delete(resource connection_res, string station_id)

   Makes a request to AV-Desk to delete station with a given ID.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_st_delete) {
    zval *res = NULL;
    int rsrc_type = 0;
    int id_len = 0;
    char *id = NULL;
    dwavdapi_handle *handle = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &res, &id, &id_len) == FAILURE) {
        return;
    }
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)    
    if(0 == id_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Station ID is not specified");
        RETURN_FALSE
    }            
    if (DWAVDAPI_FAILURE == dwavdapi_station_delete(handle, id)) {
        RETURN_FALSE
    }   
    
    RETURN_TRUE
}


/** bool dwavd_st_send_message(resource connection_res, string station_id, string message[, string link_text[, string link_url[, string logo_filepath[, string logo_link[, string logo_text]]]])

   Makes a request to AV-Desk to send a message to a given station.
   `message` - Message text which can contain {link} macros to place link given in link parameters to.
   `link_text` - URL description text to be placed in {link} macros.
   `link_url` - URL to be placed in {link} macros.
   `logo_filepath` - An absolute path to a logo .bmp file to show in a message.
   `logo_text` - Description text for a logo.
   `logo_link` - URL to go to on a logo click.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_st_send_message) {
    int rsrc_type = 0;
    zval *res = NULL;
    char *id = NULL;
    char *logo_file = NULL;
    char *logo_link = NULL;
    char *logo_text = NULL;
    char *link_text = NULL;
    char *link_url = NULL;
    char *message = NULL;
    int message_len = 0;
    int logo_file_len = 0;
    int logo_link_len = 0;
    int link_text_len = 0;
    int link_url_len = 0;
    int logo_text_len = 0;
    int id_len = 0;
    dwavdapi_message *msg = NULL;
    dwavdapi_handle *handle = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|sssss", &res, &id, &id_len, &message, &message_len, &link_text, &link_text_len, &link_url, &link_url_len, &logo_file, &logo_file_len, &logo_link, &logo_link_len, &logo_text, &logo_text_len) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_FALSE(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
            
    if (0 == id_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Station ID is not specified");
        RETURN_FALSE
    }

    if (0 == message_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Message is not specified");
        RETURN_FALSE
    }

    msg = dwavdapi_message_init();
    dwavdapi_message_set_msg(msg, message);

    if (0 < link_url_len) {
        dwavdapi_message_set_url(msg, link_url);
    }

    if (0 < link_text_len) {
        dwavdapi_message_set_url_text(msg, link_text);
    }

    if (0 < logo_file_len) {
        dwavdapi_message_set_logo(msg, logo_file);
    }

    if (0 < logo_link_len) {
        dwavdapi_message_set_logo_url(msg, logo_link);
    }

    if (0 < logo_text_len) {
        dwavdapi_message_set_logo_text(msg, logo_text);
    }

    if (DWAVDAPI_SUCCESS == dwavdapi_station_send_message(handle, id, msg)) {
        dwavdapi_message_destroy(msg);
        RETURN_TRUE
    }
    dwavdapi_message_destroy(msg);
    RETURN_FALSE
}


/** ======= Stats: Station resource ======= */

/** mixed dwavd_st_get_stats(resource connection_res, string st_id[, int ts_from[, int ts_till[, int virus_limit]]])

   Makes a request to AV-Desk to get statistics resource for a given station.
   Returns group statistics resource on success, or NULL otherwise. */
PHP_FUNCTION(dwavd_st_get_stats) {
    zval *res = NULL;
    int rsrc_type = 0;
    dwavdapi_handle *handle = NULL;
    dwavdapi_station_statistics *stats = NULL;
    long till = 0;
    long from = 0;
    long top_viruses = 0;
    char *id;
    int id_len = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rslll", &res, &id, &id_len, &from, &till, &top_viruses) == FAILURE) {
        return;
    }
    
    DWAVD_FETCH_HANDLE_WITH_RETURN_NULL(handle, Z_RESVAL_P(res), le_dwavd, rsrc_type)
            
    if(0 == id_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Station ID is not specified");
        RETURN_NULL();
    }  

    DWAVD_CHECK_TIME(from, from)
    DWAVD_CHECK_TIME(till, till)
            
    if(0 > top_viruses) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Incorrect value of `top_viruses'. Expected positive number, got negative");
        RETURN_NULL();
    }
            
    if (DWAVDAPI_FAILURE == dwavdapi_station_get_statistics(handle, &stats, id, from, till, top_viruses)) {
        RETURN_NULL();
    }     
    ZEND_REGISTER_RESOURCE(return_value, stats, le_dwavd_st_stats);
}


/** bool dwavd_st_stats_free(resource st_stats_res)

   Frees memory allocated for a given station statistics resource.
   Returns true on success, or false otherwise. */
PHP_FUNCTION(dwavd_st_stats_free) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_station_statistics *stats = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_ST_STATS_RES_WITH_RETURN_FALSE(stats, Z_RESVAL_P(res), le_dwavd_st_stats, rsrc_type)
    if (FAILURE == zend_list_delete(Z_RESVAL_P(res))) {
        RETURN_FALSE
    }
    RETURN_TRUE
}


/** mixed dwavd_st_stats(resource st_stats_res, string param_name)

   Returns a value of a given parameter from station statistics resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_st_stats) {
    int rsrc_type = 0;
    int flag = -1;
    zval *res = NULL;
    zval *opt = NULL;
    dwavdapi_station_statistics *stats = NULL;
    dwavdapi_statistics_scans scans;
    dwavdapi_statistics_infections infections;
    dwavdapi_list *list = NULL;
    dwavdapi_list *n_list = NULL;
    int rsrc = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_ST_STATS_RES_WITH_RETURN_FALSE(stats, Z_RESVAL_P(res), le_dwavd_st_stats, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_grp_stats_opt_array) 
    
    switch(flag) {       
        case DWAVD_ST_STATS_SCANS:
            scans = dwavdapi_station_statistics_scans(stats);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, &scans, le_dwavd_stats_scans)
            RETURN_RESOURCE(rsrc);     
        case DWAVD_ST_STATS_INFECTIONS:
            infections = dwavdapi_station_statistics_infections(stats);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, &infections, le_dwavd_stats_infections)
            RETURN_RESOURCE(rsrc); 
        case DWAVD_ST_STATS_VIRUSES:
            list = dwavdapi_station_statistics_viruses_list(stats);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_virus_ctor);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_viruses_list)
            RETURN_RESOURCE(rsrc); 
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_st_stats_array(resource st_stats_res)

   Converts data from station statistics resource into an array and returns it.
   Returns an array with station stats resource data on success, or false otherwise. */
PHP_FUNCTION(dwavd_st_stats_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_station_statistics *stats = NULL;
    dwavdapi_statistics_scans scans;
    dwavdapi_statistics_infections infections;
    dwavdapi_list *list = NULL;
    zval *list_array = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_ST_STATS_RES_WITH_RETURN_FALSE(stats, Z_RESVAL_P(res), le_dwavd_st_stats, rsrc_type)
    array_init(return_value);
        
    scans = dwavdapi_station_statistics_scans(stats);
    _dwavd_scans_array(&array, &scans);
    add_assoc_zval(return_value, "scans", array);
    
    infections = dwavdapi_station_statistics_infections(stats);
    _dwavd_infcd_array(&array, &infections);
    add_assoc_zval(return_value, "infections", array);
    
    list = dwavdapi_station_statistics_viruses_list(stats);
    _dwavd_res_list_to_array(&list_array TSRMLS_CC, list, le_dwavd_viruses_list);
    add_assoc_zval(return_value, "viruses", list_array);
}


/** ======= Stations states helpers ======= */

/** mixed dwavd_stats_sts_state(resource st_stats_res, string param_name)

   Returns a value of a given station state parameter from a given station resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_stats_sts_state) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_statistics_stations_state *state = NULL;
    zval *opt = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_STATES_RES_WITH_RETURN_FALSE(state, Z_RESVAL_P(res), le_dwavd_stations_state, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_st_state_opt_array)
    switch(flag) {
        case DWAVD_ST_STATE_ACTIVATED:
            RETURN_LONG(dwavdapi_statistics_stations_state_activated(state));
        case DWAVD_ST_STATE_UNACTIVATED:
            RETURN_LONG(dwavdapi_statistics_stations_state_unactivated(state));
        case DWAVD_ST_STATE_BLOCKED:
            RETURN_LONG(dwavdapi_statistics_stations_state_blocked(state));
        case DWAVD_ST_STATE_DEINSTALLED:
            RETURN_LONG(dwavdapi_statistics_stations_state_deinstalled(state));
        case DWAVD_ST_STATE_EXPIRED:
            RETURN_LONG(dwavdapi_statistics_stations_state_expired(state));
        case DWAVD_ST_STATE_OFFLINE:
            RETURN_LONG(dwavdapi_statistics_stations_state_offline(state));
        case DWAVD_ST_STATE_ONLINE:
            RETURN_LONG(dwavdapi_statistics_stations_state_online(state));
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_stats_sts_state_array(resource st_stats_res)

   Converts data from a given station states statistics resource into an array and returns it.
   Returns an array with statistics on success, or false otherwise. */
PHP_FUNCTION(dwavd_stats_sts_state_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_statistics_stations_state *state = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_STATES_RES_WITH_RETURN_FALSE(state, Z_RESVAL_P(res), le_dwavd_stations_state, rsrc_type)
    array_init(return_value);
    _dwavd_stats_array(&array, state);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}


/** ======= Stats: Scan helpers ======= */

/** mixed dwavd_stats_scans(resource stats_res, string param_name)

   Returns a value of a given parameter in scan results statistics from a given statistics resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_stats_scans) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_statistics_scans *scans = NULL;
    zval *opt = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_SCANS_RES_WITH_RETURN_FALSE(scans, Z_RESVAL_P(res), le_dwavd_stats_scans, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_scans_opt_array)
    switch(flag) {
        case DWAVD_SCANS_SIZE:
            RETURN_DOUBLE(dwavdapi_statistics_scans_size(scans));
        case DWAVD_SCANS_FILES:
            RETURN_DOUBLE(dwavdapi_statistics_scans_files(scans));
        case DWAVD_SCANS_DELETED:
            RETURN_LONG(dwavdapi_statistics_scans_deleted(scans));
        case DWAVD_SCANS_MOVED:
            RETURN_LONG(dwavdapi_statistics_scans_moved(scans));
        case DWAVD_SCANS_CURED:
            RETURN_LONG(dwavdapi_statistics_scans_cured(scans));
        case DWAVD_SCANS_ERRORS:
            RETURN_LONG(dwavdapi_statistics_scans_errors(scans));
        case DWAVD_SCANS_INFECTED:
            RETURN_LONG(dwavdapi_statistics_scans_infected(scans));
        case DWAVD_SCANS_RENAMED:
            RETURN_LONG(dwavdapi_statistics_scans_renamed(scans));
        case DWAVD_SCANS_LOCKED:
            RETURN_LONG(dwavdapi_statistics_scans_locked(scans));
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_stats_scans_array(resource stats_res)

   Converts data from a given statistics resource into an array with scan results statistics and returns it.
   Returns an array with statistics on success, or false otherwise. */
PHP_FUNCTION(dwavd_stats_scans_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_statistics_scans *scans = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_SCANS_RES_WITH_RETURN_FALSE(scans, Z_RESVAL_P(res), le_dwavd_stats_scans, rsrc_type)
    array_init(return_value);
    _dwavd_scans_array(&array, scans);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}



/** ======= Stats: Infections helpers ======= */

/** mixed dwavd_stats_infcd(resource stats_res, string param_name)

   Returns a value of a given parameter in infection statistics from a given statistics resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_stats_infcd) {
    int rsrc_type = 0;
    zval *res = NULL;
    dwavdapi_statistics_infections *infcd = NULL;
    zval *opt = NULL;
    int flag = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_INFCD_RES_WITH_RETURN_FALSE(infcd, Z_RESVAL_P(res), le_dwavd_stats_infections, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_infcd_opt_array)
    switch(flag) {
        case DWAVD_INFCD_CURED:
            RETURN_LONG(dwavdapi_statistics_infections_cured(infcd));
        case DWAVD_INFCD_DELETED:
            RETURN_LONG(dwavdapi_statistics_infections_deleted(infcd));
        case DWAVD_INFCD_ERRORS:
            RETURN_LONG(dwavdapi_statistics_infections_errors(infcd));
        case DWAVD_INFCD_IGNORED:
            RETURN_LONG(dwavdapi_statistics_infections_ignored(infcd));
        case DWAVD_INFCD_INCURABLE:
            RETURN_LONG(dwavdapi_statistics_infections_incurable(infcd));
        case DWAVD_INFCD_LOCKED:
            RETURN_LONG(dwavdapi_statistics_infections_locked(infcd));
        case DWAVD_INFCD_MOVED:
            RETURN_LONG(dwavdapi_statistics_infections_moved(infcd));
        case DWAVD_INFCD_RENAMED:
            RETURN_LONG(dwavdapi_statistics_infections_renamed(infcd));
        case DWAVD_INFCD_TOTAL:
            RETURN_LONG(dwavdapi_statistics_infections_total(infcd));            
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_stats_infcd_array(resource stats_res)

   Converts data from a given statistics resource into an array with infections statistics and returns it.
   Returns an array with statistics on success, or false otherwise. */
PHP_FUNCTION(dwavd_stats_infcd_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_statistics_infections *infcd = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_INFCD_RES_WITH_RETURN_FALSE(infcd, Z_RESVAL_P(res), le_dwavd_stats_infections, rsrc_type)
    array_init(return_value);
    _dwavd_infcd_array(&array, infcd);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}



/** ======= Stats: Traffic helpers ======= */

/** mixed dwavd_stats_traffic(resource stats_res, string param_name)

   Returns a value of a given parameter in traffic statistics from a given statistics resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_stats_traffic) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *opt = NULL;
    int flag = -1;
    dwavdapi_statistics_traffic *traffic = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_TRAFFIC_RES_WITH_RETURN_FALSE(traffic, Z_RESVAL_P(res), le_dwavd_stats_traffic, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_traffic_opt_array)
    switch(flag) {
        case DWAVD_TRAFFIC_IN:
            RETURN_DOUBLE(dwavdapi_srv_traffic_in(traffic));
        case DWAVD_TRAFFIC_OUT:
            RETURN_DOUBLE(dwavdapi_srv_traffic_out(traffic));
        case DWAVD_TRAFFIC_TOTAL:
            RETURN_DOUBLE(dwavdapi_srv_traffic_total(traffic));            
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_stats_traffic_array(resource stats_res)

   Converts data from a given statistics resource into an array with traffic statistics and returns it.
   Returns an array with statistics on success, or false otherwise. */
PHP_FUNCTION(dwavd_stats_traffic_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_statistics_traffic *traffic = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_TRAFFIC_RES_WITH_RETURN_FALSE(traffic, Z_RESVAL_P(res), le_dwavd_stats_traffic, rsrc_type)
    array_init(return_value);
    _dwavd_traffic_array(&array, traffic);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}



/** ======= Virus resource ======= */

/** mixed dwavd_virus(resource virus_res, string param_name)

   Returns a value of a given parameter from a given virus resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_virus) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *opt = NULL;
    int flag = -1;
    dwavdapi_virus *virus = NULL;
    dwavdapi_list  *list = NULL;
    dwavdapi_list  *n_list = NULL;
    int rsrc = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_VIRUS_RES_WITH_RETURN_FALSE(virus, Z_RESVAL_P(res), le_dwavd_virus, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_virus_opt_array)
    
    switch(flag) {
        case DWAVD_VIRUS_NAME:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_virus_name(virus));
        case DWAVD_VIRUS_OBJECTS:
            list = dwavdapi_virus_infected_objects_list(virus);
            dwavdapi_list_ctor(&n_list, list, (dwavdapi_ctor_funct)dwavdapi_infected_object_ctor);
            rsrc = ZEND_REGISTER_RESOURCE(NULL, n_list, le_dwavd_infcd_objs_list);
            RETURN_RESOURCE(rsrc);    

        case DWAVD_VIRUS_OBJECTS_COUNT:
            RETURN_LONG(dwavdapi_virus_infected_objects_count(virus));           
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_virus_array(resource virus_res)

   Converts data from a given virus resource into an array and returns it.
   Returns an array with virus data on success, or false otherwise. */
PHP_FUNCTION(dwavd_virus_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_virus *virus = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_VIRUS_RES_WITH_RETURN_FALSE(virus, Z_RESVAL_P(res), le_dwavd_virus, rsrc_type)
    array_init(return_value);
    _dwavd_virus_array(&array, virus);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}



/** ======= Infected object resource ======= */

/** mixed dwavd_infcd_obj(resource infcd_res, string param_name)

   Returns a value of a given parameter from a given infected object resource.
   On failure returns false. */
PHP_FUNCTION(dwavd_infcd_obj) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *opt = NULL;
    int flag = -1;
    dwavdapi_infected_object *obj = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &res, &opt) == FAILURE) {
        return;
    }
    DWAVD_FETCH_INFCD_OBJ_RES_WITH_RETURN_FALSE(obj, Z_RESVAL_P(res), le_dwavd_infcd_obj, rsrc_type)
    DWAVD_OPT_TO_FLAG(flag, opt, _dwavd_infcd_objs_opt_array)
    
    switch(flag) {
        case DWAVD_INFCD_OBJ_TYPE:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_infected_object_type_str(obj))
        case DWAVD_INFCD_OBJ_CURE:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_infected_object_cure_status_str(obj))
        case DWAVD_INFCD_OBJ_TYPE_CODE:
            RETURN_LONG(dwavdapi_infected_object_type(obj));  
        case DWAVD_INFCD_OBJ_CURE_CODE:
            RETURN_LONG(dwavdapi_infected_object_cure_status(obj)); 
        case DWAVD_INFCD_OBJ_INFECTION_TYPE:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_infection_type_str(obj))
        case DWAVD_INFCD_OBJ_INFECTION_TYPE_CODE:
            RETURN_LONG(dwavdapi_infection_type(obj));  
        case DWAVD_INFCD_OBJ_ORIGINATOR:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_application_name(dwavdapi_infection_originator(obj)))
        case DWAVD_INFCD_OBJ_ORIGINATOR_CODE:
            RETURN_LONG(dwavdapi_infection_originator(obj));
        case DWAVD_INFCD_OBJ_PATH:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_infected_object_path(obj))
        case DWAVD_INFCD_OBJ_OWNER:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_infected_object_owner(obj))
        case DWAVD_INFCD_OBJ_STATION_ID:
            DWAVD_RETURN_STRING_OR_NULL(dwavdapi_infection_station_id(obj))
        case DWAVD_INFCD_OBJ_USERNAME:
             DWAVD_RETURN_STRING_OR_NULL(dwavdapi_infected_object_username(obj))
    }
    DWAVD_UNKNOWN_OPTION(opt)
}


/** mixed dwavd_infcd_obj_array(resource infcd_res)

   Converts data from a given infected object resource into an array and returns it.
   Returns an array with infected object data on success, or false otherwise. */
PHP_FUNCTION(dwavd_infcd_obj_array) {
    int rsrc_type = 0;
    zval *res = NULL;
    zval *array = NULL;
    dwavdapi_infected_object *obj = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    DWAVD_FETCH_INFCD_OBJ_RES_WITH_RETURN_FALSE(obj, Z_RESVAL_P(res), le_dwavd_infcd_obj, rsrc_type)
    array_init(return_value);
    _dwavd_infcd_obj_array(&array, obj);
    *return_value = *array;
    zval_copy_ctor(return_value);
    INIT_PZVAL(return_value);
}

