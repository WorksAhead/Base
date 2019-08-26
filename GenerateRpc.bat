@cd Common\Rpc
@del *.h
@del *.cpp
..\..\Build\packages\zeroc.ice.v140.3.7.2\tools\slice2cpp --underscore -I . "RpcErrorCode.ice"
..\..\Build\packages\zeroc.ice.v140.3.7.2\tools\slice2cpp --underscore -I . "RpcTypedefs.ice"
..\..\Build\packages\zeroc.ice.v140.3.7.2\tools\slice2cpp --underscore -I . "RpcManagedObject.ice"
..\..\Build\packages\zeroc.ice.v140.3.7.2\tools\slice2cpp --underscore -I . "RpcDownloader.ice"
..\..\Build\packages\zeroc.ice.v140.3.7.2\tools\slice2cpp --underscore -I . "RpcUploader.ice"
..\..\Build\packages\zeroc.ice.v140.3.7.2\tools\slice2cpp --underscore -I . "RpcStart.ice"
..\..\Build\packages\zeroc.ice.v140.3.7.2\tools\slice2cpp --underscore -I . "RpcSession.ice"
@pause
