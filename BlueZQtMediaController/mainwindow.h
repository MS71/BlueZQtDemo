#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "KF5/BluezQt/bluezqt/adapter.h"
#include "KF5/BluezQt/bluezqt/device.h"
#include "KF5/BluezQt/bluezqt/pendingcall.h"
#include "KF5/BluezQt/bluezqt/initmanagerjob.h"
#include "KF5/BluezQt/bluezqt/mediaplayer.h"
#include "KF5/BluezQt/bluezqt/mediaplayertrack.h"
#include "KF5/BluezQt/bluezqt/manager.h"
#include "KF5/BluezQt/bluezqt/profile.h"

namespace Ui {
class MainWindow;
}

using namespace BluezQt;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void updateAdapters();
    void updateDevices();
    void updateDevice();
    void updateMediaPlayer();

public Q_SLOTS:
    /**
     * Indicates that operational state have changed.
     */
    void slot_manager_operationalChanged(bool operational);

    /**
     * Indicates that Bluetooth operational state have changed.
     */
    void slot_manager_bluetoothOperationalChanged(bool operational);

    /**
     * Indicates that Bluetooth blocked state have changed.
     */
    void slot_manager_bluetoothBlockedChanged(bool blocked);

    /**
     * Indicates that adapter was added.
     */
    void slot_manager_adapterAdded(AdapterPtr adapter);

    /**
     * Indicates that adapter was removed.
     */
    void slot_manager_adapterRemoved(AdapterPtr adapter);

    /**
     * Indicates that at least one of the adapter's properties have changed.
     */
    void slot_manager_adapterChanged(AdapterPtr adapter);

    /**
     * Indicates that a new device was added (eg. found by discovery).
     */
    void slot_manager_deviceAdded(DevicePtr device);

    /**
     * Indicates that a device was removed.
     */
    void slot_manager_deviceRemoved(DevicePtr device);

    /**
     * Indicates that at least one of the device's properties have changed.
     */
    void slot_manager_deviceChanged(DevicePtr device);

    /**
     * Indicates that usable adapter have changed.
     */
    void slot_manager_usableAdapterChanged(AdapterPtr adapter);

    /**
     * Indicates that all adapters were removed.
     */
    void slot_manager_allAdaptersRemoved();

    /**
     * Indicates that player's repeat state have changed.
     */
    //void slot_mediaplayer_repeatChanged(Repeat repeat);

    /**
     * Indicates that player's shuffle state have changed.
     */
    //void slot_mediaplayer_shuffleChanged(Shuffle shuffle);

    /**
     * Indicates that player's status have changed.
     */
    void slot_mediaplayer_statusChanged(MediaPlayer::Status status);

    /**
     * Indicates that player's current track have changed.
     */
    void slot_mediaplayer_trackChanged(MediaPlayerTrack track);

    /**
     * Indicates that player's playback position have changed.
     */
    void slot_mediaplayer_positionChanged(quint32 position);

private:
    Ui::MainWindow *ui;

public:
    static Manager *g_manager;
    DevicePtr       pDevice;
    MediaPlayerPtr  pMediaPlayer;

private slots:
    void on_checkBox_manager_bluetooth_blocked_clicked();
    void on_pushButton_log_clear_clicked();
    void on_pushButton_amanager_scan_clicked();
    void on_checkBox_discoverable_clicked();
    void on_comboBox_devices_currentTextChanged(const QString &arg1);
    void on_pushButton_device_pair_clicked();
    void on_pushButton_device_canclepairing_clicked();
    void on_pushButton_device_connect_clicked();
    void on_pushButton_device_disconnect_clicked();
    void on_pushButton_device_mediaplayer_repeat_clicked();
    void on_pushButton_device_mediaplayer_shuffle_clicked();
    void on_pushButton_device_mediaplayer_play_clicked();
    void on_pushButton_device_mediaplayer_pause_clicked();
    void on_pushButton_device_mediaplayer_stop_clicked();
    void on_pushButton_device_mediaplayer_next_clicked();
    void on_pushButton_device_mediaplayer_previous_clicked();
    void on_pushButton_device_mediaplayer_fastforward_clicked();
    void on_pushButton_device_mediaplayer_rewind_clicked();
};

#endif // MAINWINDOW_H
