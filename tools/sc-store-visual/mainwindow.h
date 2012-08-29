#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QListWidget;
class SegmentView;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //! Create central widgets
    void createMainWidgets();

    //! Setup action slots
    void setupActions();


protected slots:
    void openRepository();

private:

    Ui::MainWindow *ui;

    //! List of segments
    QListWidget *mSegmentsList;
    //! Widget that visualize segment
    SegmentView *mSegmentView;

};

#endif // MAINWINDOW_H
