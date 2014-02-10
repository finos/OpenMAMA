/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <mama/mamacpp.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaOrderBookListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mama/MamaQueueGroup.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <ncurses.h>

#include <signal.h>
#include <unistd.h>
#include "../parsecmd.h"
#include "../dictrequester.h"

using std::endl;
using std::vector;
using std::cerr;
using std::cout;

using namespace Wombat;

void usage  (int exitStatus);
static void start  (bool useColor);
static void finish (int sig);

static const char* sBidHeading = "            ID       Time        Size   Price |";
static const char* sAskHeading = " Price   Size        Time       ID";
static int         sMidColumn  = strlen (sBidHeading) - 1;

class BookViewer : public MamdaOrderBookHandler
                 , public MamdaErrorListener
                 , public MamdaQualityListener
                 , public MamaIoCallback
{
public:
    BookViewer (const MamdaOrderBook& book);

    virtual ~BookViewer ();

    void onBookRecap (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookComplexDelta*   delta,
        const MamdaOrderBookRecap&          recap,
        const MamdaOrderBook&               book);

    void onBookDelta (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookSimpleDelta&    delta,
        const MamdaOrderBook&               book);

    void onBookComplexDelta (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookComplexDelta&   delta,
        const MamdaOrderBook&               book);

    void onBookClear (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookClear&  event,
        const MamdaOrderBook&       book);

    void onBookGap (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookGap&    event,
        const MamdaOrderBook&       book);

    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       code,
        const char*          errorStr);

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality);

    void setShowEntries (bool  showEntries);

    void display        (const MamdaOrderBook&  book);
    void displayLevels  (const MamdaOrderBook&  book);
    void displayEntries (const MamdaOrderBook&  book);
    void displayBidEntriesInLevel (const MamdaOrderBookPriceLevel* level,
                                   int&                            i,
                                   int                             colorId);
    void displayAskEntriesInLevel (const MamdaOrderBookPriceLevel* level,
                                   int&                            i,
                                   int                             colorId);
    void displayFooter  ();
    void onIo (MamaIo* io, mamaIoType ioType);

private:
    const MamdaOrderBook&  mBook;
    bool                   mShowEntries;
};



int main (int argc, const char **argv)
{
    try
    {
        CommonCommandLineParser     cmdLine (argc, argv);
        // Initialize the MAMA API
        mamaBridge bridge = cmdLine.getBridge();
        Mama::open ();

        const vector<const char*>&  symbolList    = cmdLine.getSymbolList ();
        MamaSource*                 source        = cmdLine.getSource();
        MamaQueueGroup              queues (cmdLine.getNumThreads(), bridge);
        DictRequester               dictRequester (bridge);
        bool                        processEntries = true;

        // We might as well enforce strict checking of order book updates
        // (at the expense of some performance).
        MamdaOrderBook::setStrictChecking (true);

        signal (SIGINT, finish);         /* arrange interrupts to terminate */
        bool useColor = !cmdLine.getOptBool ('b');  /* check for blk & white */
        start (useColor);

        // Get and initialize the dictionary
        dictRequester.requestDictionary     (cmdLine.getDictSource());
        MamdaCommonFields::setDictionary    (*dictRequester.getDictionary ());
        MamdaOrderBookFields::setDictionary (*dictRequester.getDictionary ());

        const char* symbolMapFile = cmdLine.getSymbolMapFile ();
        if (symbolMapFile)
        {
            MamaSymbolMapFile* aMap = new MamaSymbolMapFile;
            if (MAMA_STATUS_OK == aMap->load (symbolMapFile))
            {
                source->getTransport()->setSymbolMap (aMap);
            }
        }

        int subscCount = 0;

        for (vector<const char*>::const_iterator i = symbolList.begin ();
             i != symbolList.end ();
             ++i)
        {
            const char* symbol = *i;
            MamdaSubscription*      aSubscription = new MamdaSubscription;
            MamdaOrderBookListener* aBookListener = new MamdaOrderBookListener;
            BookViewer* aViewer = 
                new BookViewer (*aBookListener->getOrderBook());

            aBookListener->setProcessEntries  (true);
            aBookListener->addHandler         (aViewer);
            aSubscription->addMsgListener     (aBookListener);
            aSubscription->addQualityListener (aViewer);
            aSubscription->addErrorListener   (aViewer);
            aSubscription->setType            (MAMA_SUBSC_TYPE_BOOK);
            aSubscription->setMdDataType      (MAMA_MD_DATA_TYPE_ORDER_BOOK);
            aSubscription->create             (queues.getNextQueue (),
                                               source,
                                               symbol);

            aViewer->setShowEntries (false);

            (new MamaIo ())->create (queues.getNextQueue (),
                                aViewer,
                                0,  /* stdin */
                                MAMA_IO_READ);
        }

        Mama::start (bridge);
    }
    catch (MamaStatus &e)
    {
        /*
           This exception can be thrown from Mama::start ().
           Mama::createTransport (transportName) and from
           MamdaSubscription constructor when entitlements is enabled.
        */
        cerr << "MamaStatus exception in main (): " << e.toString () << endl;
        exit (1);
    }
    catch (std::exception &ex)
    {
        cerr << "Exception in main (): " << ex.what () << endl;
        exit (1);
    }
    catch (...)
    {
        cerr << "Unknown Exception in main ()." << endl;
        exit (1);
    }

    return 1;
}

void usage (int exitStatus)
{
    std::cerr << "Usage: bookviewer -m middleware [-S source] "
              << "-s symbol [-s symbol ...] " 
              << "[-b] (For black and white) [-threads x]\n";
    exit (exitStatus);
}

void start (bool useColor)
{
    initscr ();  /* start curses mode */
    keypad (stdscr, true);
    raw ();
    noecho ();
    if (useColor)
    {
        start_color ();
        use_default_colors ();
        init_pair (1, COLOR_RED,     COLOR_WHITE);
        init_pair (2, COLOR_MAGENTA, COLOR_WHITE);
        init_pair (3, COLOR_BLUE,    COLOR_WHITE);
        init_pair (4, COLOR_CYAN,    COLOR_WHITE);
        init_pair (5, COLOR_GREEN,   COLOR_WHITE);
    }
}

void finish (int sig)
{
    endwin ();
    exit (0);
}

BookViewer::BookViewer (const MamdaOrderBook& book)
    : mBook (book)
    , mShowEntries (false)
{
}

BookViewer::~BookViewer ()
{
    endwin ();   /* end curses mode */
}

void BookViewer::onBookRecap (
    MamdaSubscription*                  subscription,
    MamdaOrderBookListener&             listener,
    const MamaMsg*                      msg,
    const MamdaOrderBookComplexDelta*   delta,
    const MamdaOrderBookRecap&          recap,
    const MamdaOrderBook&               book)
{
    display (book);
}

void BookViewer::onBookDelta (
    MamdaSubscription*                  subscription,
    MamdaOrderBookListener&             listener,
    const MamaMsg*                      msg,
    const MamdaOrderBookSimpleDelta&    delta,
    const MamdaOrderBook&               book)
{
    display (book);
}

void BookViewer::onBookComplexDelta (
    MamdaSubscription*                  subscription,
    MamdaOrderBookListener&             listener,
    const MamaMsg*                      msg,
    const MamdaOrderBookComplexDelta&   delta,
    const MamdaOrderBook&               book)
{
    display (book);
}

void BookViewer::onBookClear (
    MamdaSubscription*          subscription,
    MamdaOrderBookListener&     listener,
    const MamaMsg*              msg,
    const MamdaOrderBookClear&  event,
    const MamdaOrderBook&       book)
{
    display (book);
}

void BookViewer::onBookGap (
    MamdaSubscription*          subscription,
    MamdaOrderBookListener&     listener,
    const MamaMsg*              msg,
    const MamdaOrderBookGap&    event,
    const MamdaOrderBook&       book)
{
    cout << "GAP!!!\n";
    cout << "Book gap (" << event.getBeginGapSeqNum () << "-" 
         << event.getEndGapSeqNum () << ")\n";
}

void BookViewer::onError (
    MamdaSubscription*   subscription,
    MamdaErrorSeverity   severity,
    MamdaErrorCode       code,
    const char*          errorStr)
{
    cout << "bookticker: ERROR: " << errorStr << "\n";
}

void BookViewer::onQuality (
    MamdaSubscription*   subscription,
    mamaQuality          quality)
{
    cout << "bookticker: QUALITY: " << quality << "\n";
}

void BookViewer::setShowEntries (bool  showEntries)
{
    mShowEntries = showEntries;
}

void BookViewer::display (const MamdaOrderBook&  book)
{
    if (mShowEntries)
        displayEntries (book);
    else
        displayLevels (book);
    displayFooter ();
}

void BookViewer::displayLevels (const MamdaOrderBook&  book)
{
    const char* symbol = book.getSymbol ();
    int symbolCol = sMidColumn - (strlen (symbol)/2);
    attron (A_BOLD);
    mvprintw (0, symbolCol, "%s", symbol);
    mvprintw (1, 0, "%s | %s",
             "        Time     Num    Size   Price",
             "Price   Size    Num       Time  ");
    attroff (A_BOLD);

    MamdaOrderBook::constBidIterator bidIter = book.bidBegin ();
    MamdaOrderBook::constBidIterator bidEnd  = book.bidEnd ();
    MamdaOrderBook::constAskIterator askIter = book.askBegin ();
    MamdaOrderBook::constAskIterator askEnd  = book.askEnd ();

    for (int i = 2; i < LINES-1; i++)
    {
        move (i, 0);
        if (i < 7)
            attron (COLOR_PAIR (i-1));
        if (bidIter != bidEnd)
        {
            const MamdaOrderBookPriceLevel* bidLevel = *bidIter;
            printw ("   %12s %4d %7d %7.4f ",
                    bidLevel->getTime ().getTimeAsString (),
                    bidLevel->getNumEntries (),
                    (long)bidLevel->getSize (),
                    bidLevel->getPrice ());
            ++bidIter;
        }
        else
        {
            printw ("                                     ");
        }

        printw ("|");

        if (askIter!= askEnd)
        {
            const MamdaOrderBookPriceLevel* askLevel = *askIter;
            printw (" %-7.4f %-7d %-6d %-12s   ",
                    askLevel->getPrice (),
                    (long)askLevel->getSize (),
                    askLevel->getNumEntries (),
                    askLevel->getTime ().getTimeAsString ());
            ++askIter;
        }
        else
        {
            printw ("                                     ");
        }
        if (i < 7)
            attroff (COLOR_PAIR (i-1));
    }
    refresh ();  // refresh the screen
}

void BookViewer::displayEntries (const MamdaOrderBook&  book)
{
    const char* symbol = book.getSymbol ();
    int symbolCol = sMidColumn - (strlen (symbol)/2);
    attron (A_BOLD);
    mvprintw (0, symbolCol, "%s", symbol);
    attroff (A_BOLD);

    /* Bid entries */
    attron (A_BOLD);
    mvprintw (1, 0, "%s", sBidHeading);
    attroff (A_BOLD);
    MamdaOrderBook::constBidIterator bidIter = book.bidBegin ();
    MamdaOrderBook::constBidIterator bidEnd  = book.bidEnd ();
    int colorId = 1;
    for (int i = 2; i < LINES-1;)
    {
        if (bidIter != bidEnd)
        {
            const MamdaOrderBookPriceLevel* bidLevel = *bidIter;
            displayBidEntriesInLevel (bidLevel, i, colorId);
            ++bidIter;
            ++colorId;
        }
        else
        {
            mvprintw (i, 0, "                                              |");
            i++;
        }
    }

    /* Ask entries */
    attron (A_BOLD);
    mvprintw (1, sMidColumn+1, "%s", sAskHeading);
    attroff (A_BOLD);
    MamdaOrderBook::constAskIterator askIter = book.askBegin ();
    MamdaOrderBook::constAskIterator askEnd  = book.askEnd ();
    colorId = 1;
    for (int i = 2; i < LINES-1;)
    {
        if (askIter!= askEnd)
        {
            const MamdaOrderBookPriceLevel* askLevel = *askIter;
            displayAskEntriesInLevel (askLevel, i, colorId);
            ++askIter;
            ++colorId;
        }
        else
        {
            mvprintw (i, sMidColumn+1,
                     "                                              ");
            i++;
        }
    }

    refresh ();  // refresh the screen
}

void BookViewer::displayBidEntriesInLevel (
    const MamdaOrderBookPriceLevel*  level,
    int&                             i,
    int                              colorId)
{
    MamdaOrderBookPriceLevel::iterator end  = level->end ();
    MamdaOrderBookPriceLevel::iterator iter = level->begin ();
    while ((iter != end) && (i < LINES-1))
    {
        move (i, 0);
        if ((0 < colorId) && (colorId <= 5))
            attron (COLOR_PAIR (colorId));
        const MamdaOrderBookEntry* entry = *iter;
        printw (" %14s  %12s %7d %7.4f ",
                entry->getId (), entry->getTime ().getTimeAsString (),
                (long)entry->getSize (), level->getPrice ());
        if ((0 < colorId) && (colorId <= 5))
            attroff (COLOR_PAIR (colorId));
        printw ("|");
        ++iter;
        ++i;
    }
}

void BookViewer::displayAskEntriesInLevel (
    const MamdaOrderBookPriceLevel*  level,
    int&                             i,
    int                              colorId)
{
    MamdaOrderBookPriceLevel::iterator end  = level->end ();
    MamdaOrderBookPriceLevel::iterator iter = level->begin ();
    while ((iter != end) && (i < LINES-1))
    {
        move (i, sMidColumn+1);
        if ((0 < colorId) && (colorId <= 5))
            attron (COLOR_PAIR (colorId));
        const MamdaOrderBookEntry* entry = *iter;
        printw (" %-7.4f %-7d %-12s  %-14s ",
                level->getPrice (), (long)entry->getSize (),
                entry->getTime ().getTimeAsString (), entry->getId ());
        if ((0 < colorId) && (colorId <= 5))
            attroff (COLOR_PAIR (colorId));
        ++iter;
        ++i;
    }
}

void BookViewer::displayFooter ()
{
    attron (A_BOLD);
    mvprintw (LINES-1, 1, "%s", "Q-Quit  E-ToggleEntries  CTRL-L-ScreenRefresh");
    attroff (A_BOLD);
}

void BookViewer::onIo (MamaIo* io, mamaIoType ioType)
{
    // Handle keyboard events.
    int ch;
    switch (ch = getch ())
    {
        case 0x03: // CTRL-C
        case 'q':
        case 'Q':
            // Quit
            endwin ();
            exit (0);
            break;
        case 0x0C: // CTRL-L
            // Refresh screen
            clear ();
            display (mBook);
            break;
        case 'e':
        case 'E':
            // Toggle display of entries
            mShowEntries = !mShowEntries;
            clear ();
            display (mBook);
            break;
        default:
            printw ("unhandled: %c\n", ch);
            refresh ();
    }
}
