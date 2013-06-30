#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class QListWidget;
class QLabel;
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
    void setupMenuActions();

private:
    void updateSegmentsList();


protected slots:
    void openRepository();

    void segmentSelectionChanged(QString strId);

private:

    Ui::MainWindow *ui;

    //! List of segments
    QListWidget *mSegmentsList;
    //! Widget that visualize segment
    SegmentView *mSegmentView;

    QLabel *mLabelNodeCount;
    QLabel *mLabelArcsCount;
    QLabel *mLabelEmptyCount;

};

#endif // MAINWINDOW_H
