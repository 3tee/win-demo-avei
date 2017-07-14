#ifndef RT_Error_H_
#define RT_Error_H_

#include "combase.h"

/* Standard "it worked" return value；成功返回值 */
#define	AVD_OK			0
#define RT_FAILED(rv) (rv != AVD_OK)
#define RT_SUCCEEDED(rv) (rv == AVD_OK)

//client error code
#define	Err_Base		1000
/* Returned when a function fails */
#define	Err_Failure				(Result)(Err_Base + 1)
/* Returned when an instance is not initialized */
#define Err_Not_Initialized		(Result)(Err_Base + 2)
/* Returned when an instance is already initialized */
#define Err_Already_Initialized (Result)(Err_Base + 3)
/* Returned by a not implemented function */
#define Err_Not_Implemented		(Result)(Err_Base + 4)
/* Returned when null pointer occures */
#define Err_Null_Pointer		(Result)(Err_Base + 5)
/* Returned when an unexpected error occurs */
#define Err_UnExpected			(Result)(Err_Base + 6)
/* Returned when a memory allocation fails */
#define Err_Out_Of_Memory		(Result)(Err_Base + 7)
/* Returned when an illegal value is passed */
#define Err_Invalid_Arg			(Result)(Err_Base + 8)
/* Returned when an operation can't complete due to an unavailable resource */
#define Err_Not_Available		(Result)(Err_Base + 9)
/* object cann't find */
#define Err_Not_Found			(Result)(Err_Base + 11)
/* operation timeout */
#define Err_TimeOut				(Result)(Err_Base + 14)
/* object in wrong status */
#define Err_Wrong_Status		(Result)(Err_Base + 15)
/* network failure */
#define Err_Network_Failure		(Result)(Err_Base + 16)
/* token is none */
#define Err_No_Token			(Result)(Err_Base + 17)
/* video input: image convert failed. */
#define Err_ImageConvert_Failed	(Result)(Err_Base + 18)
/* need more buff */
#define Err_Morebuff_Needed		(Result)(Err_Base + 19)
/* device already occupied */
#define Err_DeviceOccupied		(Result)(Err_Base + 20)
/* operation has done */
#define Err_Operation_Completed	(Result)(Err_Base + 21)
/* function unauthorized */
#define Err_Function_Unauthorized (Result)(Err_Base + 25)
/* mcu_url is empty in Cluster version mcu*/
#define Err_Cluster_Invalid		(Result)(Err_Base + 26)
/* mcu connection failure*/
#define Err_McuConnection_Failure		(Result)(Err_Base + 27)
/* publish or priview or setOption(eo_camera_capability_default) not support resolution */
#define Err_Video_NotSupportResolution	(Result)(Err_Base + 28)
/* room closed*/
#define Err_Room_Close					(Result)(Err_Base + 29)
/* stream connection timeout */
#define Err_Stream_Connection_TimeOut	(Result)(Err_Base + 30)
/* get_mcu failed in cluster server */
#define Err_Rest_GetMcu_Failure			(Result)(Err_Base + 31)
/* room signal channel connect failed */
#define Err_Net_ChannelSignal_Failure	(Result)(Err_Base + 32)
/* room data channel connect failed */
#define Err_Net_ChannelData_Failure		(Result)(Err_Base + 33)
/* no data, wait for data ok.  */
#define Err_Waiting_Data				(Result)(Err_Base + 34)

// for importer
#define Err_Importer_Video_Failure	(Result)(Err_Base + 80)
#define Err_Importer_Audio_Failure	(Result)(Err_Base + 81)
#define Err_Importer_BeKicked		(Result)(Err_Base + 82)


//server error code
#define	Err_Base_Srv			0
/* client is unauthorized */
#define Err_Unauthorized		(Result)(Err_Base_Srv + 401)
/* client is rejoin error */
#define Err_Rejoin				(Result)(Err_Base_Srv + 402)
/* room not found */
#define Err_Room_None			(Result)(Err_Base_Srv + 404) 
/* license expired */
#define Err_License_Expired		(Result)(Err_Base_Srv + 405)
/* mcu not register */
#define Err_Mcu_NoRegister		(Result)(Err_Base_Srv + 406)
/* http request param invalid */
#define Err_Rest_InvalidParam	(Result)(Err_Base_Srv + 407)
/* http request not supported */
#define Err_Rest_Notsupported	(Result)(Err_Base_Srv + 408)
/* http request timeout */
#define Err_Rest_TimeOut		(Result)(Err_Base_Srv + 409)
/* none available server */
#define Err_Server_NoAvailable	(Result)(Err_Base_Srv + 410)
/* server out of max user  */
#define Err_Server_OutofMaxUser	(Result)(Err_Base_Srv + 411)
/* server is box version */
#define Err_Server_BoxVersion	(Result)(Err_Base_Srv + 412)
/* room is in used */
#define Err_Room_InUsed			(Result)(Err_Base_Srv + 413)
/* schedule service not available */
#define Err_Schedule_Failed		(Result)(Err_Base_Srv + 414)
/* none mcu for roomId */
#define Err_NoMcu_ByRoomId		(Result)(Err_Base_Srv + 415)
/* mcu not found room */
#define Err_NoRoom_InMcu		(Result)(Err_Base_Srv + 416)
/* mcu is offline */
#define Err_Mcu_Offline			(Result)(Err_Base_Srv + 417)
/* master rpc timeout */
#define Err_Master_RpcTimeout	(Result)(Err_Base_Srv + 418)
/* cluster have no master */
#define Err_Cluster_NoMaster	(Result)(Err_Base_Srv + 419)
/* cluster error */
#define Err_Cluster_Error		(Result)(Err_Base_Srv + 420)
/* server is not leader */
#define Err_Server_NotLeader	(Result)(Err_Base_Srv + 421)
/* mcu server is not exist */
#define Err_Server_McuNone		(Result)(Err_Base_Srv + 422)
/* service unavailable */
#define Err_Service_Unavailable	(Result)(Err_Base_Srv + 503)
/* server time out */
#define Err_Server_TimeOut		(Result)(Err_Base_Srv + 504)
/* version not support */
#define Err_Version_Invalid		(Result)(Err_Base_Srv + 505)
/* server out of user */
#define Err_Server_OutofUser	(Result)(Err_Base_Srv + 506)
/* room out of max user */
#define Err_Room_OutofUser		(Result)(Err_Base_Srv + 507)
/* license out of max user */
#define Err_License_OutofUser	(Result)(Err_Base_Srv + 508)
/* app room id invalid */
#define Err_RoomId_Invalid		(Result)(Err_Base_Srv + 601)
/* authentication failed */
#define Err_Auth_Failed			(Result)(Err_Base_Srv + 602)
/* get mcu not found room */
#define Err_GetMcu_NoRoom		(Result)(Err_Base_Srv + 612)
/* mcu proxy connect up server failed */
#define Err_Proxy_UpConn_Failed	(Result)(Err_Base_Srv + 613)
/* database error: data invalid */
#define Err_Database_Error		(Result)(Err_Base_Srv + 700)	
/* function no permission */
#define Err_Server_NoPermition	(Result)(Err_Base_Srv + 701)	
/* room out of max video */
#define Err_Room_OutofVideo		(Result)(Err_Base_Srv + 800)
/* room out of max audio */
#define Err_Room_OutofAudio		(Result)(Err_Base_Srv + 801)	
/* duplicate publish */
#define Err_Duplicate_Publish	(Result)(Err_Base_Srv + 802)	
/* room token invalid */
#define Err_Room_ErrToken		(Result)(Err_Base_Srv + 803)	
/* duplicate userid join, kick off prejoin user. */
#define Err_User_DuplicateJoin	(Result)(Err_Base_Srv + 804)
/* server internal error. */
#define Err_Server_InternalError	(Result)(Err_Base_Srv + 805)	
/* server database connection timeout. */
#define Err_Server_DBConnTimeout	(Result)(Err_Base_Srv + 806)	
/* data connection connect failed, kick off user. */
#define Err_User_DataConnection_Failed	(Result)(Err_Base_Srv + 807)	


#endif//RT_Error_H_
