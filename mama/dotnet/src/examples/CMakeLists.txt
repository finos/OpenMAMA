cmake_minimum_required (VERSION 2.8.3)
project (mamacs)

csharp_build_project(MamaFtMember MamaFtMember/MamaFtMemberCS.csproj MamaFtMember mamacs)
csharp_build_project(MamaInbox MamaInbox/MamaInboxCS.csproj MamaInbox mamacs)
csharp_build_project(MamaIo MamaIo/MamaIoCS.csproj MamaIo mamacs)
csharp_build_project(MamaListen MamaListen/MamaListenCS.csproj MamaListen mamacs)
csharp_build_project(MamaMultiSubscriber MamaMultiSubscriber/MamaMultiSubscriberCS.csproj MamaMultiSubscriber mamacs)
csharp_build_project(MamaPublisher MamaPublisher/MamaPublisherCS.csproj MamaPublisher mamacs)
csharp_build_project(MamaSubscriber MamaSubscriber/MamaSubscriberCS.csproj MamaSubscriber mamacs)
csharp_build_project(MamaSymbolListSubscriber MamaSymbolListSubscriber/MamaSymbolListSubscriberCS.csproj MamaSymbolListSubscriber mamacs)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MamaFtMember.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaInbox.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaIo.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaListen.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaMultiSubscriber.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaPublisher.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaSubscriber.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamaSymbolListSubscriber.exe
         DESTINATION bin)
