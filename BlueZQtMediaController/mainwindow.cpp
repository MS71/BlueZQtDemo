#include "mainwindow.h"
#include "ui_mainwindow.h"

Manager* MainWindow::g_manager = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->textEdit_log->append("Init ...");
    if( g_manager == nullptr )
    {
        g_manager = new Manager();
        if( g_manager != nullptr )
        {
            ui->textEdit_log->append("Init Manager ...");
            InitManagerJob *initJob = g_manager->init();
            if( initJob != nullptr )
            {
                initJob->exec();
            }
            connect(g_manager,&Manager::operationalChanged,this,&MainWindow::slot_manager_operationalChanged);
            connect(g_manager,&Manager::bluetoothOperationalChanged,this,&MainWindow::slot_manager_bluetoothOperationalChanged);
            connect(g_manager,&Manager::bluetoothBlockedChanged,this,&MainWindow::slot_manager_bluetoothBlockedChanged);

            connect(g_manager,&Manager::adapterAdded,this,&MainWindow::slot_manager_adapterAdded);
            connect(g_manager,&Manager::adapterRemoved,this,&MainWindow::slot_manager_adapterRemoved);
            connect(g_manager,&Manager::adapterChanged,this,&MainWindow::slot_manager_adapterChanged);
            connect(g_manager,&Manager::deviceAdded,this,&MainWindow::slot_manager_deviceAdded);
            connect(g_manager,&Manager::deviceRemoved,this,&MainWindow::slot_manager_deviceRemoved);
            connect(g_manager,&Manager::deviceChanged,this,&MainWindow::slot_manager_deviceChanged);
            connect(g_manager,&Manager::usableAdapterChanged,this,&MainWindow::slot_manager_usableAdapterChanged);
            connect(g_manager,&Manager::allAdaptersRemoved,this,&MainWindow::slot_manager_allAdaptersRemoved);

            pDevice.clear();
            pMediaPlayer.clear();
            updateDevice();

            slot_manager_allAdaptersRemoved();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateDevices()
{
    ui->textEdit_devices->clear();
    QString c = ui->comboBox_devices->currentText();
    ui->comboBox_devices->clear();
    if( g_manager != nullptr )
    {
        for ( DevicePtr d : g_manager->devices() )
        {
            ui->textEdit_devices->append(d->name());
            ui->comboBox_devices->addItem( d.data()->address() + " (" + d->name() + ")" );
        }
    }
    if( c != "" )
    {
        ui->comboBox_devices->setCurrentText(c);
    }
}

void MainWindow::updateAdapters()
{
    ui->textEdit_adapters->clear();
    if( g_manager != nullptr )
    {
        for ( AdapterPtr a : g_manager->adapters() )
        {
            ui->textEdit_adapters->append(a->name());
        }
    }
    updateDevices();
}

void MainWindow::updateDevice()
{
    if( !pDevice.isNull() )
    {
        ui->groupBox_device->setEnabled(true);
        ui->label_device_name->setText("Name: " + pDevice->name() + " (" + pDevice->friendlyName() + ")");
        ui->label_device_address->setText("Addr: " + pDevice->address());
        ui->label_device_type->setText("Type: " + pDevice->typeToString(pDevice->type()));
        ui->label_device_ubi->setText("Ubi: " + pDevice->ubi());
        ui->label_device_modalias->setText("MODAlias: " + pDevice->modalias());
        ui->comboBox_device_uuids->clear();
        for ( QString s : pDevice->uuids() )
        {
            ui->comboBox_device_uuids->addItem(s);
        }
        ui->label_device_uuids->setText("UUIDs: ");
        ui->checkBox_device_paired->setChecked(pDevice->isPaired());
        ui->checkBox_device_connected->setChecked(pDevice->isConnected());

        pMediaPlayer = pDevice->mediaPlayer();
        if( !pMediaPlayer.isNull() )
        {
            //connect(pMediaPlayer,&MediaPlayer::repeatChanged,this,&MainWindow::slot_mediaplayer_repeatChanged);
            //connect(pMediaPlayer,&MediaPlayer::shuffleChanged,this,&MainWindow::slot_mediaplayer_shuffleChanged);
            connect(pMediaPlayer.data(),&MediaPlayer::statusChanged,this,&MainWindow::slot_mediaplayer_statusChanged);
            connect(pMediaPlayer.data(),&MediaPlayer::trackChanged,this,&MainWindow::slot_mediaplayer_trackChanged);
            connect(pMediaPlayer.data(),&MediaPlayer::positionChanged,this,&MainWindow::slot_mediaplayer_positionChanged);
        }
    }
    else
    {
        ui->groupBox_device->setEnabled(false);
        ui->label_device_name->setText("Name: ");
        ui->label_device_address->setText("Addr: ");
        ui->label_device_type->setText("Type: ");
        ui->label_device_ubi->setText("Ubi: ");
        ui->label_device_modalias->setText("MODAlias: ");
        ui->comboBox_device_uuids->clear();
        ui->label_device_uuids->setText("UUIDs: ");
        ui->checkBox_device_paired->setChecked(false);
        ui->checkBox_device_connected->setChecked(false);
    }

    updateMediaPlayer();
}

void MainWindow::updateMediaPlayer()
{
    if( !pMediaPlayer.isNull() )
    {
        ui->label_device_mediaplayer_name->setText("Name: " + pMediaPlayer->name());
        ui->label_device_mediaplayer_position->setText("Position: " + QString().number(pMediaPlayer->position()));
        ui->label_device_mediaplayer_status->setText("Status: " + QString().number(pMediaPlayer->status()));
        ui->textEdit_device_mediaplayer_track->clear();
        if( pMediaPlayer->track().isValid() )
        {
            ui->textEdit_device_mediaplayer_track->append(QString().number(pMediaPlayer->track().trackNumber()) + "/" + QString().number(pMediaPlayer->track().numberOfTracks()) + ": (" + QString().number(pMediaPlayer->track().duration())+ ")");
            ui->textEdit_device_mediaplayer_track->append("Genre: " + pMediaPlayer->track().genre());
            ui->textEdit_device_mediaplayer_track->append("Artist: " + pMediaPlayer->track().artist());
            ui->textEdit_device_mediaplayer_track->append("Album: " + pMediaPlayer->track().album());
            ui->textEdit_device_mediaplayer_track->append("Title: " + pMediaPlayer->track().title());
        }
    }
    else
    {
        ui->label_device_mediaplayer_name->setText("Name: -");
        ui->label_device_mediaplayer_position->setText("Position: -");
        ui->label_device_mediaplayer_status->setText("Status: -");
        ui->textEdit_device_mediaplayer_track->clear();
        ui->pushButton_device_mediaplayer_repeat->setText("Repeat Off");
        ui->pushButton_device_mediaplayer_shuffle->setText("Shuffle Off");
    }
}

/**
 * Indicates that operational state have changed.
 */
void MainWindow::slot_manager_operationalChanged(bool operational)
{
    ui->checkBox_manager_operational->setChecked(operational);
}

/**
 * Indicates that Bluetooth operational state have changed.
 */
void MainWindow::slot_manager_bluetoothOperationalChanged(bool operational)
{
    ui->checkBox_manager_bluetooth_operational->setChecked(operational);
}

/**
 * Indicates that Bluetooth blocked state have changed.
 */
void MainWindow::slot_manager_bluetoothBlockedChanged(bool blocked)
{
    ui->checkBox_manager_bluetooth_blocked->setChecked(blocked);
}

/**
 * Indicates that adapter was added.
 */
void MainWindow::slot_manager_adapterAdded(AdapterPtr adapter)
{
    updateAdapters();
}

/**
 * Indicates that adapter was removed.
 */
void MainWindow::slot_manager_adapterRemoved(AdapterPtr adapter)
{
    updateAdapters();
}

/**
 * Indicates that at least one of the adapter's properties have changed.
 */
void MainWindow::slot_manager_adapterChanged(AdapterPtr adapter)
{
    updateAdapters();
}

/**
 * Indicates that a new device was added (eg. found by discovery).
 */
void MainWindow::slot_manager_deviceAdded(DevicePtr device)
{
    updateDevices();
}

/**
 * Indicates that a device was removed.
 */
void MainWindow::slot_manager_deviceRemoved(DevicePtr device)
{
    updateDevices();
}

/**
 * Indicates that at least one of the device's properties have changed.
 */
void MainWindow::slot_manager_deviceChanged(DevicePtr device)
{
    updateDevices();
}

/**
 * Indicates that usable adapter have changed.
 */
void MainWindow::slot_manager_usableAdapterChanged(AdapterPtr adapter)
{
    updateAdapters();
}

/**
 * Indicates that all adapters were removed.
 */
void MainWindow::slot_manager_allAdaptersRemoved()
{
    updateAdapters();
}

/**
 * Indicates that Bluetooth blocked state have changed.
 */
void MainWindow::on_checkBox_manager_bluetooth_blocked_clicked()
{
    if( g_manager != nullptr )
    {
        if( ui->checkBox_manager_bluetooth_blocked->isChecked() != g_manager->isBluetoothBlocked() )
        {
            g_manager->setBluetoothBlocked(ui->checkBox_manager_bluetooth_blocked->isChecked());
        }
    }
}

///
/// \brief MainWindow::on_pushButton_log_clear_clicked
///
void MainWindow::on_pushButton_log_clear_clicked()
{
    ui->textEdit_log->clear();
}

void MainWindow::on_pushButton_amanager_scan_clicked()
{
    if( g_manager != nullptr )
    {
        for ( AdapterPtr a : g_manager->adapters() )
        {
            a->startDiscovery();
        }
    }
}

void MainWindow::on_checkBox_discoverable_clicked()
{
    if( g_manager != nullptr )
    {
        for ( AdapterPtr a : g_manager->adapters() )
        {
            a->setDiscoverable(ui->checkBox_discoverable->isChecked());
        }
    }
}

void MainWindow::on_comboBox_devices_currentTextChanged(const QString &devaddrstr)
{
    if(( g_manager != nullptr )&&( devaddrstr.size() != 0 ))
    {
        DevicePtr d = g_manager->deviceForAddress(devaddrstr.split(" ").at(0));
        if( !d.isNull() )
        {
            pDevice = d;
        }
        else
        {
            pDevice.clear();
        }
    }
    else
    {
        pDevice.clear();
    }
    updateDevice();
}

void MainWindow::on_pushButton_device_pair_clicked()
{
    if( !pDevice.isNull() )
    {
        pDevice->pair();
    }
    updateDevice();
}

void MainWindow::on_pushButton_device_canclepairing_clicked()
{
    if( !pDevice.isNull() )
    {
        pDevice->cancelPairing();
    }
    updateDevice();
}

void MainWindow::on_pushButton_device_connect_clicked()
{
    if( !pDevice.isNull() )
    {
        pDevice->connectToDevice();
    }
    updateDevice();
}

void MainWindow::on_pushButton_device_disconnect_clicked()
{
    if( !pDevice.isNull() )
    {
        pDevice->disconnectFromDevice();
    }
    updateDevice();
}

void MainWindow::on_pushButton_device_mediaplayer_repeat_clicked()
{
}

void MainWindow::on_pushButton_device_mediaplayer_shuffle_clicked()
{

}

void MainWindow::on_pushButton_device_mediaplayer_play_clicked()
{
    if( !pMediaPlayer.isNull() )
    {
        pMediaPlayer->next();
    }
    updateMediaPlayer();
}

void MainWindow::on_pushButton_device_mediaplayer_pause_clicked()
{
    if( !pMediaPlayer.isNull() )
    {
        pMediaPlayer->pause();
    }
    updateMediaPlayer();
}

void MainWindow::on_pushButton_device_mediaplayer_stop_clicked()
{
    if( !pMediaPlayer.isNull() )
    {
        pMediaPlayer->stop();
    }
    updateMediaPlayer();
}

void MainWindow::on_pushButton_device_mediaplayer_next_clicked()
{
    if( !pMediaPlayer.isNull() )
    {
        pMediaPlayer->next();
    }
    updateMediaPlayer();
}

void MainWindow::on_pushButton_device_mediaplayer_previous_clicked()
{
    if( !pMediaPlayer.isNull() )
    {
        pMediaPlayer->previous();
    }
    updateMediaPlayer();
}

void MainWindow::on_pushButton_device_mediaplayer_fastforward_clicked()
{
    if( !pMediaPlayer.isNull() )
    {
        pMediaPlayer->fastForward();
    }
    updateMediaPlayer();

}

void MainWindow::on_pushButton_device_mediaplayer_rewind_clicked()
{
    if( !pMediaPlayer.isNull() )
    {
        pMediaPlayer->rewind();
    }
    updateMediaPlayer();
}

#if 0
/**
 * Indicates that player's repeat state have changed.
 */
void MainWindow::slot_mediaplayer_repeatChanged(Repeat repeat)
{
    updateMediaPlayer();
}

/**
 * Indicates that player's shuffle state have changed.
 */
void MainWindow::slot_mediaplayer_shuffleChanged(Shuffle shuffle)
{
    updateMediaPlayer();
}
#endif

/**
 * Indicates that player's status have changed.
 */
void MainWindow::slot_mediaplayer_statusChanged(MediaPlayer::Status status)
{
    updateMediaPlayer();
}

/**
 * Indicates that player's current track have changed.
 */
void MainWindow::slot_mediaplayer_trackChanged(MediaPlayerTrack track)
{
    updateMediaPlayer();
}

/**
 * Indicates that player's playback position have changed.
 */
void MainWindow::slot_mediaplayer_positionChanged(quint32 position)
{
    updateMediaPlayer();
}
