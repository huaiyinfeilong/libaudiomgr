

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for AudioPolicyConfig.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0628 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __AudioPolicyConfig_h__
#define __AudioPolicyConfig_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __IAudioPolicyConfig_FWD_DEFINED__
#define __IAudioPolicyConfig_FWD_DEFINED__
typedef interface IAudioPolicyConfig IAudioPolicyConfig;

#endif 	/* __IAudioPolicyConfig_FWD_DEFINED__ */


/* header files for imported files */
#include "inspectable.h"
#include "MMDeviceApi.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IAudioPolicyConfig_INTERFACE_DEFINED__
#define __IAudioPolicyConfig_INTERFACE_DEFINED__

/* interface IAudioPolicyConfig */
/* [object][uuid] */ 


EXTERN_C const IID IID_IAudioPolicyConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ab3d4648-e242-459f-b02f-541c70306324")
    IAudioPolicyConfig : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE __incomplete__add_CtxVolumeChange( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__remove_CtxVolumeChanged( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__add_RingerVibrateStateChanged( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__remove_RingerVibrateStateChange( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__SetVolumeGroupGainForId( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__GetVolumeGroupGainForId( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__GetActiveVolumeGroupForEndpointId( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__GetVolumeGroupsForEndpoint( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__GetCurrentVolumeContext( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__SetVolumeGroupMuteForId( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__GetVolumeGroupMuteForId( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__SetRingerVibrateState( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__GetRingerVibrateState( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__SetPreferredChatApplication( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__ResetPreferredChatApplication( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__GetPreferredChatApplication( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__GetCurrentChatApplications( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__add_ChatContextChanged( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE __incomplete__remove_ChatContextChanged( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPersistedDefaultAudioEndpoint( 
            /* [in] */ UINT processId,
            /* [in] */ EDataFlow flow,
            /* [in] */ ERole role,
            /* [in] */ HSTRING deviceId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPersistedDefaultAudioEndpoint( 
            /* [in] */ UINT processId,
            /* [in] */ EDataFlow flow,
            /* [in] */ ERole role,
            /* [retval][out] */ HSTRING *pDeviceId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearAllPersistedApplicationDefaultEndpoints( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAudioPolicyConfigVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAudioPolicyConfig * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IInspectable, GetIids)
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            IAudioPolicyConfig * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        DECLSPEC_XFGVIRT(IInspectable, GetRuntimeClassName)
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            IAudioPolicyConfig * This,
            /* [out] */ HSTRING *className);
        
        DECLSPEC_XFGVIRT(IInspectable, GetTrustLevel)
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            IAudioPolicyConfig * This,
            /* [out] */ TrustLevel *trustLevel);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__add_CtxVolumeChange)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__add_CtxVolumeChange )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__remove_CtxVolumeChanged)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__remove_CtxVolumeChanged )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__add_RingerVibrateStateChanged)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__add_RingerVibrateStateChanged )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__remove_RingerVibrateStateChange)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__remove_RingerVibrateStateChange )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__SetVolumeGroupGainForId)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__SetVolumeGroupGainForId )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__GetVolumeGroupGainForId)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__GetVolumeGroupGainForId )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__GetActiveVolumeGroupForEndpointId)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__GetActiveVolumeGroupForEndpointId )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__GetVolumeGroupsForEndpoint)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__GetVolumeGroupsForEndpoint )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__GetCurrentVolumeContext)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__GetCurrentVolumeContext )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__SetVolumeGroupMuteForId)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__SetVolumeGroupMuteForId )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__GetVolumeGroupMuteForId)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__GetVolumeGroupMuteForId )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__SetRingerVibrateState)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__SetRingerVibrateState )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__GetRingerVibrateState)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__GetRingerVibrateState )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__SetPreferredChatApplication)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__SetPreferredChatApplication )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__ResetPreferredChatApplication)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__ResetPreferredChatApplication )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__GetPreferredChatApplication)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__GetPreferredChatApplication )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__GetCurrentChatApplications)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__GetCurrentChatApplications )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__add_ChatContextChanged)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__add_ChatContextChanged )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, __incomplete__remove_ChatContextChanged)
        HRESULT ( STDMETHODCALLTYPE *__incomplete__remove_ChatContextChanged )( 
            IAudioPolicyConfig * This);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, SetPersistedDefaultAudioEndpoint)
        HRESULT ( STDMETHODCALLTYPE *SetPersistedDefaultAudioEndpoint )( 
            IAudioPolicyConfig * This,
            /* [in] */ UINT processId,
            /* [in] */ EDataFlow flow,
            /* [in] */ ERole role,
            /* [in] */ HSTRING deviceId);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, GetPersistedDefaultAudioEndpoint)
        HRESULT ( STDMETHODCALLTYPE *GetPersistedDefaultAudioEndpoint )( 
            IAudioPolicyConfig * This,
            /* [in] */ UINT processId,
            /* [in] */ EDataFlow flow,
            /* [in] */ ERole role,
            /* [retval][out] */ HSTRING *pDeviceId);
        
        DECLSPEC_XFGVIRT(IAudioPolicyConfig, ClearAllPersistedApplicationDefaultEndpoints)
        HRESULT ( STDMETHODCALLTYPE *ClearAllPersistedApplicationDefaultEndpoints )( 
            IAudioPolicyConfig * This);
        
        END_INTERFACE
    } IAudioPolicyConfigVtbl;

    interface IAudioPolicyConfig
    {
        CONST_VTBL struct IAudioPolicyConfigVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAudioPolicyConfig_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAudioPolicyConfig_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAudioPolicyConfig_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAudioPolicyConfig_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define IAudioPolicyConfig_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define IAudioPolicyConfig_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define IAudioPolicyConfig___incomplete__add_CtxVolumeChange(This)	\
    ( (This)->lpVtbl -> __incomplete__add_CtxVolumeChange(This) ) 

#define IAudioPolicyConfig___incomplete__remove_CtxVolumeChanged(This)	\
    ( (This)->lpVtbl -> __incomplete__remove_CtxVolumeChanged(This) ) 

#define IAudioPolicyConfig___incomplete__add_RingerVibrateStateChanged(This)	\
    ( (This)->lpVtbl -> __incomplete__add_RingerVibrateStateChanged(This) ) 

#define IAudioPolicyConfig___incomplete__remove_RingerVibrateStateChange(This)	\
    ( (This)->lpVtbl -> __incomplete__remove_RingerVibrateStateChange(This) ) 

#define IAudioPolicyConfig___incomplete__SetVolumeGroupGainForId(This)	\
    ( (This)->lpVtbl -> __incomplete__SetVolumeGroupGainForId(This) ) 

#define IAudioPolicyConfig___incomplete__GetVolumeGroupGainForId(This)	\
    ( (This)->lpVtbl -> __incomplete__GetVolumeGroupGainForId(This) ) 

#define IAudioPolicyConfig___incomplete__GetActiveVolumeGroupForEndpointId(This)	\
    ( (This)->lpVtbl -> __incomplete__GetActiveVolumeGroupForEndpointId(This) ) 

#define IAudioPolicyConfig___incomplete__GetVolumeGroupsForEndpoint(This)	\
    ( (This)->lpVtbl -> __incomplete__GetVolumeGroupsForEndpoint(This) ) 

#define IAudioPolicyConfig___incomplete__GetCurrentVolumeContext(This)	\
    ( (This)->lpVtbl -> __incomplete__GetCurrentVolumeContext(This) ) 

#define IAudioPolicyConfig___incomplete__SetVolumeGroupMuteForId(This)	\
    ( (This)->lpVtbl -> __incomplete__SetVolumeGroupMuteForId(This) ) 

#define IAudioPolicyConfig___incomplete__GetVolumeGroupMuteForId(This)	\
    ( (This)->lpVtbl -> __incomplete__GetVolumeGroupMuteForId(This) ) 

#define IAudioPolicyConfig___incomplete__SetRingerVibrateState(This)	\
    ( (This)->lpVtbl -> __incomplete__SetRingerVibrateState(This) ) 

#define IAudioPolicyConfig___incomplete__GetRingerVibrateState(This)	\
    ( (This)->lpVtbl -> __incomplete__GetRingerVibrateState(This) ) 

#define IAudioPolicyConfig___incomplete__SetPreferredChatApplication(This)	\
    ( (This)->lpVtbl -> __incomplete__SetPreferredChatApplication(This) ) 

#define IAudioPolicyConfig___incomplete__ResetPreferredChatApplication(This)	\
    ( (This)->lpVtbl -> __incomplete__ResetPreferredChatApplication(This) ) 

#define IAudioPolicyConfig___incomplete__GetPreferredChatApplication(This)	\
    ( (This)->lpVtbl -> __incomplete__GetPreferredChatApplication(This) ) 

#define IAudioPolicyConfig___incomplete__GetCurrentChatApplications(This)	\
    ( (This)->lpVtbl -> __incomplete__GetCurrentChatApplications(This) ) 

#define IAudioPolicyConfig___incomplete__add_ChatContextChanged(This)	\
    ( (This)->lpVtbl -> __incomplete__add_ChatContextChanged(This) ) 

#define IAudioPolicyConfig___incomplete__remove_ChatContextChanged(This)	\
    ( (This)->lpVtbl -> __incomplete__remove_ChatContextChanged(This) ) 

#define IAudioPolicyConfig_SetPersistedDefaultAudioEndpoint(This,processId,flow,role,deviceId)	\
    ( (This)->lpVtbl -> SetPersistedDefaultAudioEndpoint(This,processId,flow,role,deviceId) ) 

#define IAudioPolicyConfig_GetPersistedDefaultAudioEndpoint(This,processId,flow,role,pDeviceId)	\
    ( (This)->lpVtbl -> GetPersistedDefaultAudioEndpoint(This,processId,flow,role,pDeviceId) ) 

#define IAudioPolicyConfig_ClearAllPersistedApplicationDefaultEndpoints(This)	\
    ( (This)->lpVtbl -> ClearAllPersistedApplicationDefaultEndpoints(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAudioPolicyConfig_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HSTRING_UserSize(     unsigned long *, unsigned long            , HSTRING * ); 
unsigned char * __RPC_USER  HSTRING_UserMarshal(  unsigned long *, unsigned char *, HSTRING * ); 
unsigned char * __RPC_USER  HSTRING_UserUnmarshal(unsigned long *, unsigned char *, HSTRING * ); 
void                      __RPC_USER  HSTRING_UserFree(     unsigned long *, HSTRING * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


