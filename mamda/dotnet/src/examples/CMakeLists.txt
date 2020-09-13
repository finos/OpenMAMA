csharp_build_project(MamdaAtomicBookTicker MamdaAtomicBookTicker/MamdaAtomicBookTickerCS.csproj MamdaAtomicBookTicker mamdacs)
csharp_build_project(MamdaAuctionTicker MamdaAuctionTicker/MamdaAuctionTickerCS.csproj MamdaAuctionTicker mamdacs)
csharp_build_project(MamdaBookChurn MamdaBookChurn/MamdaBookChurnCS.csproj MamdaBookChurn mamdacs)
csharp_build_project(MamdaBookTicker MamdaBookTicker/MamdaBookTickerCS.csproj MamdaBookTicker mamdacs)
csharp_build_project(MamdaComboTicker MamdaComboTicker/MamdaComboTickerCS.csproj MamdaComboTicker mamdacs)
csharp_build_project(MamdaFundamentalTicker MamdaFundamentalTicker/MamdaFundamentalTickerCS.csproj MamdaFundamentalTicker mamdacs)
csharp_build_project(MamdaListen MamdaListen/MamdaListenCS.csproj MamdaListen mamdacs)
csharp_build_project(MamdaMultiPartTicker MamdaMultiPartTicker/MamdaMultiPartTickerCS.csproj MamdaMultiPartTicker mamdacs)
csharp_build_project(MamdaMultiSecurityTicker MamdaMultiSecurityTicker/MamdaMultiSecurityTickerCS.csproj MamdaMultiSecurityTicker mamdacs)
csharp_build_project(MamdaOptionChainExample MamdaOptionChainExample/MamdaOptionChainExampleCS.csproj MamdaOptionChainExample mamdacs)
csharp_build_project(MamdaOptionChainViewExample MamdaOptionChainViewExample/MamdaOptionChainViewExampleCS.csproj MamdaOptionChainViewExample mamdacs)
csharp_build_project(MamdaQuoteTicker MamdaQuoteTicker/MamdaQuoteTickerCS.csproj MamdaQuoteTicker mamdacs)
csharp_build_project(MamdaSecStatusTicker MamdaSecStatusTicker/MamdaSecStatusTickerCS.csproj MamdaSecStatusTicker mamdacs)
csharp_build_project(MamdaTradeTicker MamdaTradeTicker/MamdaTradeTickerCS.csproj MamdaTradeTicker mamdacs)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MamdaAtomicBookTicker.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaAuctionTicker.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaBookChurn.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaBookTicker.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaComboTicker.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaFundamentalTicker.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaListen.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaMultiPartTicker.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaMultiSecurityTicker.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaOptionChainExample.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaOptionChainViewExample.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaQuoteTicker.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaSecStatusTicker.exe
               ${CMAKE_CURRENT_BINARY_DIR}/MamdaTradeTicker.exe
         DESTINATION bin)
