#include "MyWinBitTorrent.h"
#include "TorrentTask.h"

int main(int argc, char **argv)
{
    InitNetwork();
    CTorrentTask clTask;
    clTask.LoadTorrentFile("ubuntu-15.04-desktop-i386.iso.torrent");
    clTask.SetDstPath("D:\\Downloads");
    clTask.Startup();
    while(1)
    {

    }
//     clTask.Shutdown();
//     StopNetwork();
    return 0;
}