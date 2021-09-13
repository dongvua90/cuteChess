#ifndef NEROAPP_H
#define NEROAPP_H

#include <QApplication>
#include <board/side.h>
#include <board/genericmove.h>
#include <board/result.h>
class EngineManager;
class GameManager;
class MainWindow;
class SettingsDialog;
class TournamentResultsDialog;
class GameDatabaseManager;
class GameDatabaseDialog;
class PgnImporter;
class ChessGame;
class GameWall;
namespace Chess {
    class Board;
    class Move;
}

class NeroApp : public QApplication
{
    Q_OBJECT
public:
    NeroApp(int& argc, char* argv[]);
    GameManager* gameManager();
    static NeroApp *instance();
private slots:
    void humanEnabled(bool);
    void fenChanged(const QString& fenString);
    void moveMade(const Chess::GenericMove& move,
              const QString& sanString,
              const QString& comment);
    void moveChanged(int ply,
             const Chess::GenericMove& move,
             const QString& sanString,
             const QString& comment);
    void scoreChanged(int ply, int score);
    void initialized(ChessGame* game = nullptr);
    void started(ChessGame* game = nullptr);
    void finished(ChessGame* game = nullptr,
              Chess::Result result = Chess::Result());
    void startFailed(ChessGame* game = nullptr);
    void playersReady();
private:
    GameManager* m_gameManager;


};

#endif // NEROAPP_H
