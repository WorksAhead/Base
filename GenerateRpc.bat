@cd Common\Rpc
@del *.h
@del *.cpp
..\..\Tools\slice2cpp --underscore -I . "RpcErrorCode.ice"
..\..\Tools\slice2cpp --underscore -I . "RpcStart.ice"
..\..\Tools\slice2cpp --underscore -I . "RpcSession.ice"
@pause
