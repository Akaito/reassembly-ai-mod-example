
#ifndef EXPORT_H
#define EXPORT_H

#include "Interface.h"

bool isAppSandboxed();

inline string buildExportName(const string &fname0, const string &fname1)
{
    return "Reassembly_" + str_path_sanitize(str_replace(fname0, " ", "_")) + "_" + str_timestamp() + fname1;
}

bool startFleetExport(const SaveData &sd, const char* name);
bool startShipExport(const BlockCluster *cl);

bool getExportOutput(string &msg);

bool getFileExportName(string &fname, const char *ext);

void ImportFleetShip(GameState *self, SaveData *opbp, BlockCluster **opship);

#endif // EXPORT_H
