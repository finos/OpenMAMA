msbuild_project(MamdaAtomicBookTicker MamdaAtomicBookTicker/MamdaAtomicBookTickerCS.csproj MamdaAtomicBookTicker)
msbuild_project(MamdaAuctionTicker MamdaAuctionTicker/MamdaAuctionTickerCS.csproj MamdaAuctionTicker)
msbuild_project(MamdaBookChurn MamdaBookChurn/MamdaBookChurnCS.csproj MamdaBookChurn)
msbuild_project(MamdaBookTicker MamdaBookTicker/MamdaBookTickerCS.csproj MamdaBookTicker)
msbuild_project(MamdaComboTicker MamdaComboTicker/MamdaComboTickerCS.csproj MamdaComboTicker)
msbuild_project(MamdaFundamentalTicker MamdaFundamentalTicker/MamdaFundamentalTickerCS.csproj MamdaFundamentalTicker)
msbuild_project(MamdaListen MamdaListen/MamdaListenCS.csproj MamdaListen)
msbuild_project(MamdaMultiPartTicker MamdaMultiPartTicker/MamdaMultiPartTickerCS.csproj MamdaMultiPartTicker)
msbuild_project(MamdaMultiSecurityTicker MamdaMultiSecurityTicker/MamdaMultiSecurityTickerCS.csproj MamdaMultiSecurityTicker)
msbuild_project(MamdaOptionChainExample MamdaOptionChainExample/MamdaOptionChainExampleCS.csproj MamdaOptionChainExample)
msbuild_project(MamdaOptionChainViewExample MamdaOptionChainViewExample/MamdaOptionChainViewExampleCS.csproj MamdaOptionChainViewExample)
msbuild_project(MamdaQuoteTicker MamdaQuoteTicker/MamdaQuoteTickerCS.csproj MamdaQuoteTicker)
msbuild_project(MamdaSecStatusTicker MamdaSecStatusTicker/MamdaSecStatusTickerCS.csproj MamdaSecStatusTicker)
msbuild_project(MamdaTradeTicker MamdaTradeTicker/MamdaTradeTickerCS.csproj MamdaTradeTicker)

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
