

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
#include "unknwn.h"
#include "MMDeviceApi.h"
#include "inspectable.h"

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


