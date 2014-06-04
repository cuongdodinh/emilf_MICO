#include "mico_callback.h"

typedef struct _Notify_list{
  void  *function;
  struct _Notify_list *next;
} _Notify_list_t;

static mico_Context_t * _Context;

_Notify_list_t* Notify_list[20] = {NULL};

/* MICO system defined notifications */
typedef void (*mico_notify_WIFI_SCAN_COMPLETE_function)           ( ScanResult *pApList, mico_Context_t * inContext );
typedef void (*mico_notify_WIFI_STATUS_CHANGED_function)          ( WiFiEvent status, mico_Context_t * inContext );
typedef void (*mico_notify_WiFI_PARA_CHANGED_function)            ( apinfo_adv_t *ap_info, char *key, int key_len, mico_Context_t * inContext );
typedef void (*mico_notify_DHCP_COMPLETE_function)                ( net_para_st *pnet, mico_Context_t * inContext );
typedef void (*mico_notify_EASYLINK_COMPLETE_function)            ( network_InitTypeDef_st *nwkpara, mico_Context_t * inContext );
typedef void (*mico_notify_EASYLINK_GET_EXTRA_DATA_function)      ( int datalen, char*data, mico_Context_t * inContext );
typedef void (*mico_notify_TCP_CLIENT_CONNECTED_function)         ( int fd, mico_Context_t * inContext );
typedef void (*mico_notify_DNS_RESOLVE_COMPLETED_function)        ( uint8_t *hostname, uint32_t ip, mico_Context_t * inContext );
typedef void (*mico_notify_READ_APP_INFO_function)                ( char *str, int len, mico_Context_t * inContext );
typedef void (*mico_notify_SYS_WILL_POWER_OFF_function)           ( mico_Context_t * inContext );

/* User defined notifications */

void ApListCallback(ScanResult *pApList)
{
  _Notify_list_t *temp =  Notify_list[mico_notify_WIFI_SCAN_COMPLETED];
  if(temp == NULL)
    return;
  else{
    do{
      ((mico_notify_WIFI_SCAN_COMPLETE_function)(temp->function))(pApList, _Context);
      temp = temp->next;
    }while(temp!=NULL);
  }
}

void WifiStatusHandler(WiFiEvent status)
{
  _Notify_list_t *temp =  Notify_list[mico_notify_WIFI_STATUS_CHANGED];
  if(temp == NULL)
    return;
  else{
    do{
      ((mico_notify_WIFI_STATUS_CHANGED_function)(temp->function))(status, _Context);
      temp = temp->next;
    }while(temp!=NULL);
  }
}

void connected_ap_info(apinfo_adv_t *ap_info, char *key, int key_len)
{
  _Notify_list_t *temp =  Notify_list[mico_notify_WiFI_PARA_CHANGED];
  if(temp == NULL)
    return;
  else{
    do{
      ((mico_notify_WiFI_PARA_CHANGED_function)(temp->function))(ap_info, key, key_len, _Context);
      temp = temp->next;
    }while(temp!=NULL);
  }
}

void NetCallback(net_para_st *pnet)
{
  _Notify_list_t *temp =  Notify_list[mico_notify_DHCP_COMPLETED];
  if(temp == NULL)
    return;
  else{
    do{
      ((mico_notify_DHCP_COMPLETE_function)(temp->function))(pnet, _Context);
      temp = temp->next;
    }while(temp!=NULL);
  }
}

void RptConfigmodeRslt(network_InitTypeDef_st *nwkpara)
{
  _Notify_list_t *temp =  Notify_list[mico_notify_EASYLINK_COMPLETED];
  if(temp == NULL)
    return;
  else{
    do{
      ((mico_notify_EASYLINK_COMPLETE_function)(temp->function))(nwkpara, _Context);
      temp = temp->next;
    }while(temp!=NULL);
  }
}

void easylink_user_data_result(int datalen, char*data)
{
  _Notify_list_t *temp =  Notify_list[mico_notify_EASYLINK_GET_EXTRA_DATA];
  if(temp == NULL)
    return;
  else{
    do{
      ((mico_notify_EASYLINK_GET_EXTRA_DATA_function)(temp->function))(datalen, data, _Context);
      temp = temp->next;
    }while(temp!=NULL);
  }  
}

void socket_connected(int fd)
{
  _Notify_list_t *temp =  Notify_list[mico_notify_TCP_CLIENT_CONNECTED];
  if(temp == NULL)
    return;
  else{
    do{
      ((mico_notify_TCP_CLIENT_CONNECTED_function)(temp->function))(fd, _Context);
      temp = temp->next;
    }while(temp!=NULL);
  }    
}

void dns_ip_set(uint8_t *hostname, uint32_t ip)
{
  _Notify_list_t *temp =  Notify_list[mico_notify_DNS_RESOLVE_COMPLETED];
  if(temp == NULL)
    return;
  else{
    do{
      ((mico_notify_DNS_RESOLVE_COMPLETED_function)(temp->function))(hostname, ip, _Context);
      temp = temp->next;
    }while(temp!=NULL);
  }    
}


void system_version(char *str, int len){
  _Notify_list_t *temp =  Notify_list[mico_notify_READ_APP_INFO];
  if(Notify_list[mico_notify_READ_APP_INFO] == NULL)
    return;
  else{
    do{
      ((mico_notify_READ_APP_INFO_function)(temp->function))(str, len, _Context);
      temp = temp->next;
    }while(temp!=NULL);
  }
}

void sendNotifySYSWillPowerOff(void)
{
  _Notify_list_t *temp =  Notify_list[mico_notify_SYS_WILL_POWER_OFF];
  if(temp == NULL)
    return;
  else{
    do{
      ((mico_notify_SYS_WILL_POWER_OFF_function)(temp->function))(_Context);
      temp = temp->next;
    }while(temp!=NULL);
  }    
}


OSStatus mico_init_notification_center  ( void * const inContext )
{
  OSStatus err = kNoErr;
  require_action(inContext, exit, err = kParamErr);
  _Context = inContext;
exit:
  return err;
}

OSStatus mico_add_notification( mico_notify_types_t notify_type, void *functionAddress )
{
  OSStatus err = kNoErr;
  _Notify_list_t *temp =  Notify_list[notify_type];
  _Notify_list_t *notify = (_Notify_list_t *)malloc(sizeof(_Notify_list_t));
  require_action(notify, exit, err = kNoMemoryErr);
  notify->function = functionAddress;
  notify->next = NULL;
  if(Notify_list[notify_type] == NULL){
    Notify_list[notify_type] = notify;
    notify->next = NULL;
  }else{
    if(temp->function == functionAddress)
        return kNoErr;   //Nodify already exist
    while(temp->next!=NULL){
      temp = temp->next;
      if(temp->function == functionAddress)
        return kNoErr;   //Nodify already exist
    }
    temp->next = notify;
  }
exit:
  return err;
}

OSStatus mico_remove_notification( mico_notify_types_t notify_type, void *functionAddress )
{
  OSStatus err = kNoErr;
  _Notify_list_t *temp2;
  _Notify_list_t *temp = Notify_list[notify_type];
  require_action(Notify_list[notify_type], exit, err = kDeletedErr);
  do{
    if(temp->function == functionAddress){
      if(temp == Notify_list[notify_type]){  //first element
        Notify_list[notify_type] = Notify_list[notify_type]->next;
        free(temp);
      }else{
        temp2->next = temp->next;
        free(temp);
      }
       break;
    }
    require_action(temp->next!=NULL, exit, err = kNotFoundErr);
    temp2 = temp;
    temp = temp->next;
  }while(temp->next!=NULL);

exit:
  return err;
}


// void WatchDog(void)
// {
  
// }


// void RptConfigmodeRslt(network_InitTypeDef_st *nwkpara)
// {

// }


// void NetCallback(net_para_st *pnet)
// {

// }


// void ApListCallback(UwtPara_str *pApList)
// {

// }


// void dns_ip_set(u8 *hostname, u32 ip)
// {

// }


// void socket_connected(int fd)
// {

// }