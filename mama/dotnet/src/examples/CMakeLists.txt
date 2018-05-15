cmake_minimum_required (VERSION 2.8.3)
project (mamacs)

msbuild_project(MamaFtMember MamaFtMember/MamaFtMemberCS.csproj MamaFtMember)
msbuild_project(MamaInbox MamaInbox/MamaInboxCS.csproj MamaInbox)
msbuild_project(MamaIo MamaIo/MamaIoCS.csproj MamaIo)
msbuild_project(MamaListen MamaListen/MamaListenCS.csproj MamaListen)
msbuild_project(MamaMultiSubscriber MamaMultiSubscriber/MamaMultiSubscriberCS.csproj MamaMultiSubscriber)
msbuild_project(MamaPublisher MamaPublisher/MamaPublisherCS.csproj MamaPublisher)
msbuild_project(MamaSubscriber MamaSubscriber/MamaSubscriberCS.csproj MamaSubscriber)
msbuild_project(MamaSymbolListSubscriber MamaSymbolListSubscriber/MamaSymbolListSubscriberCS.csproj MamaSymbolListSubscriber)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MamaFtMember.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaInbox.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaIo.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaListen.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaMultiSubscriber.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaPublisher.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaSubscriber.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaSymbolListSubscriber.exe
         DESTINATION bin)
