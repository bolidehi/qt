#include "settings.h"
#include "ui_settings.h"

Settings::Settings(Player *Player, QWidget *parent) :
    QDialog(parent), a_isNewPath(false), a_isUpdateHandlerAlreadyCalled(false), a_previousTabId(0),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);


    //Make the Player received as an attribute so we can use it
    a_passerelle = Player;
    a_moveAnim.setDuration(MoveAnimation::Fast); //Prepare animation
    //Restore current config from QSettings
    a_settings = new QSettings("neuPlayer.ini", QSettings::IniFormat, this);
    a_settings->beginGroup("Additional_Features");
    ui->a_libraryAtStartupActivate->setChecked(a_settings->value("libraryAtStartup", false).toBool());
    ui->a_refreshWhenNeededActivate->setChecked(a_settings->value("refreshWhenNeeded", true).toBool());
    ui->a_staticLibraryActivate->setChecked(a_settings->value("staticLibrary", false).toBool());
    ui->a_saveIndexActivate->setChecked(a_settings->value("saveTrackIndex", true).toBool());
    ui->a_framelessCheck->setChecked(a_settings->value("framelessWindow", false).toBool());
    ui->a_fadeCheck->setChecked(a_settings->value("audioFade", false).toBool());
    a_settings->endGroup();
    a_bgPath = a_settings->value("customimage", "").toString();
    a_opacityValue = a_settings->value("opacity", 100).toReal();
    ui->a_opacitySlide->setValue(a_opacityValue * 100);
    on_fadeClicked(ui->a_fadeCheck->isChecked());
    updateFadeValue(a_settings->value("fadeValue", 1).toInt() / 1000);
    ui->a_fadeSlide->setValue(a_settings->value("fadeValue", 1).toInt() / 1000);
    ui->a_valueSlide->setText(QString::number(ui->a_opacitySlide->value()) + "%");
    ui->groupBox_5->setVisible(false); //Not ready for prime time yet
    //Process config into UI
    setupConfig();

    setupConnections();
}
                /* Setup Section */
void Settings::setupConfig()
{
    ui->a_playlistAtStartupCheck->setChecked(a_settings->value("playlistAtStartup", false).toBool());
    ui->a_skinPick->setCurrentIndex(a_settings->value("skin", 1).toInt());
    setSkin(ui->a_skinPick->currentIndex());
    a_isDynamicLibChecked = ui->a_refreshWhenNeededActivate->isChecked();
    a_isLibraryAtStartchecked = ui->a_libraryAtStartupActivate->isChecked();
    a_isStaticLibChecked = ui->a_staticLibraryActivate->isChecked();
    ui->a_pathView->setText(a_settings->value("mediapath", "").toString());
    ui->a_pathView->setToolTip(ui->a_pathView->text());
    if(ui->a_libraryAtStartupActivate->isChecked())
    {
        a_isLibraryAtStartchecked = true;
        ui->label_8->setHidden(false);
        ui->a_refreshWhenNeededActivate->setHidden(false);
        ui->a_staticLibraryActivate->setHidden(false);
        ui->a_saveIndexActivate->setHidden(false);
        ui->label_13->setHidden(false);
        ui->label_9->setHidden(false);
    }

    else
    {
        a_isLibraryAtStartchecked = false;
        ui->label_8->setHidden(true);
        ui->a_refreshWhenNeededActivate->setHidden(true);
        ui->a_staticLibraryActivate->setHidden(true);
        ui->label_9->setHidden(true);
        ui->a_saveIndexActivate->setHidden(true);
        ui->label_13->setHidden(true);
    }
}

void Settings::setupConnections()
{
    /* Connexions */
    connect(ui->a_libraryAtStartupActivate, SIGNAL(clicked()), this, SLOT(enableLibraryAtStartup()));
    connect(ui->a_staticLibraryActivate, SIGNAL(clicked()), this, SLOT(on_staticLibActivated()));
    connect(ui->a_refreshWhenNeededActivate, SIGNAL(clicked()), this, SLOT(on_RNlibActivated()));
    connect(ui->a_fadeCheck, SIGNAL(clicked(bool)), this, SLOT(on_fadeClicked(bool)));
    connect(ui->a_skinPick, SIGNAL(currentIndexChanged(int)), this, SLOT(setSkin(int)));
    connect(ui->a_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_changeTab(int)));
    connect(ui->a_changeDbpush, SIGNAL(clicked()), this, SLOT(changeMusicPath()));
    connect(ui->a_close_2, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->a_close, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->a_close_3, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->a_refreshNeededpush, SIGNAL(clicked()), this, SLOT(setLibrary()));
    connect(ui->a_changeBackgroundPush, SIGNAL(clicked()), this, SLOT(changeBg()));
    connect(ui->a_reloadDefaultSkinPush, SIGNAL(clicked()), this, SLOT(reloadDefaultBg()));
    connect(ui->a_opacitySlide, SIGNAL(valueChanged(int)), this, SLOT(updateOpacity(int)));
    connect(ui->a_fadeSlide, SIGNAL(valueChanged(int)), this, SLOT(updateFadeValue(int)));
    connect(ui->a_confirm, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(ui->a_confirm_2, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(ui->a_confirm_3, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(ui->a_framelessCheck, SIGNAL(clicked()), this, SLOT(popupFramelessWindow()));
    connect(ui->a_gotoWorkingDir, SIGNAL(clicked()), this, SLOT(gotoWorkingDir()));
    connect(ui->a_checkUpdatesBtn, SIGNAL(clicked()), this, SLOT(checkUpdates()));
    connect(ui->a_issueLink, SIGNAL(clicked()), this, SLOT(gotoIssues()));
}


                /* Library Section */
void Settings::enableLibraryAtStartup()
{
    if(ui->a_libraryAtStartupActivate->isChecked())
    {
        if(a_settings->value("mediapath", "").toString().isEmpty())
            changeMusicPath();
        //Re-check if user set something
        if(a_settings->value("mediapath", "").toString().isEmpty())
            return;
        a_isLibraryAtStartchecked = true;
        ui->label_8->setHidden(false);
        ui->a_refreshWhenNeededActivate->setHidden(false);
        ui->a_refreshWhenNeededActivate->setChecked(true);
        ui->a_staticLibraryActivate->setHidden(false);
        ui->a_saveIndexActivate->setHidden(false);
        ui->label_13->setHidden(false);
        ui->label_9->setHidden(false);
        ui->a_refreshNeededpush->setHidden(false);
        ui->a_changeDbpush->setHidden(false);

    }
    else if(!ui->a_libraryAtStartupActivate->isChecked())
    {
        a_isLibraryAtStartchecked = false;
        ui->label_8->setHidden(true);
        ui->a_refreshWhenNeededActivate->setHidden(true);
        ui->label_9->setHidden(true);
        ui->a_saveIndexActivate->setHidden(true);
        ui->label_13->setHidden(true);
        ui->a_refreshNeededpush->setHidden(true);
        ui->a_changeDbpush->setHidden(true);
    }
}

void Settings::on_RNlibActivated()
{
    if(ui->a_refreshWhenNeededActivate->isChecked())
    {
        ui->a_staticLibraryActivate->setChecked(false);
        a_isStaticLibChecked = false;
        a_isDynamicLibChecked = true;
    }
    else
        a_isDynamicLibChecked = false;
}

void Settings::on_staticLibActivated()
{
    if(ui->a_staticLibraryActivate->isChecked())
    {
        ui->a_refreshWhenNeededActivate->setChecked(false);
        a_isDynamicLibChecked = false;
        a_isStaticLibChecked = true;
    }
    else
        a_isStaticLibChecked = false;
}

void Settings::popupFramelessWindow()
{
    if(ui->a_framelessCheck->isChecked())
    {
        QMessageBox msgbox(QMessageBox::NoIcon, tr("neuPlayer"), tr("Vous devez red??marrer pour utiliser le player sans bordures"));
        msgbox.setWindowFlags(Qt::WindowStaysOnTopHint);
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::Ignore);
        msgbox.setButtonText(QMessageBox::Yes, tr("Red??marrer"));
        msgbox.setButtonText(QMessageBox::Ignore, tr("Plus tard"));
        int result = msgbox.exec();
        if(result == QMessageBox::Yes)
        {
            confirm();
            a_passerelle->saveBeforeClosing();
            qApp->quit();
            QProcess::startDetached("neuPlayer.exe");
        }
    }
    else
    {
        QMessageBox msgbox(QMessageBox::NoIcon, tr("neuPlayer"), tr("Vous devez red??marrer pour utiliser le player avec ses bordures"));
        msgbox.setWindowFlags(Qt::WindowStaysOnTopHint);
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::Ignore);
        msgbox.setButtonText(QMessageBox::Yes, tr("Red??marrer"));
        msgbox.setButtonText(QMessageBox::Ignore, tr("Plus tard"));
        msgbox.raise();
        int result = msgbox.exec();
        if(result == QMessageBox::Yes)
        {
            confirm();
            a_passerelle->restart();
        }
    }
}

void Settings::on_changeTab(int tabId)
{
    a_moveAnim.setTarget(ui->a_tabWidget->currentWidget());
    if(tabId < a_previousTabId)
        a_moveAnim.setDirection(MoveAnimation::RightToLeft);
    else
        a_moveAnim.setDirection(MoveAnimation::LeftToRight);
    a_moveAnim.start();
    a_previousTabId = tabId;
}

void Settings::changeMusicPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Selectionnez votre r??pertoire de musique"),"", QFileDialog::DontResolveSymlinks);
    if (path.isEmpty())
        return;
    else
    {
        if(path == ui->a_pathView->text())
            a_isNewPath = false;
        else
        {
            a_isNewPath = true;
            a_settings->setValue("mediapath", path);
            if(a_isLibraryAtStartchecked && (a_isDynamicLibChecked || a_isStaticLibChecked))
            {
                a_settings->setValue("currentTrack", 0);
                a_settings->setValue("trackPosition", 0);
            }
        }
        ui->a_pathView->setText(path);
        ui->a_pathView->setToolTip(path);
        if(!path.isEmpty() && !ui->a_libraryAtStartupActivate->isChecked()) //If the user didn't check but selected something...
        {
            ui->a_libraryAtStartupActivate->setChecked(true); //...Check the library at startup and...
            enableLibraryAtStartup(); //...Enable it
        }
    }
}

                    /* Skin Section */

void Settings::setSkin(int index)
{
    if (index == 0) //Clean Fusion
    {
        ui->a_skinImage->setPixmap(QPixmap(":/Ressources/skinpreviewlight.png"));
        if(a_bgPath.isEmpty())
            ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewwhite.jpg"));
        else
            ui->a_skinImage_2->setPixmap(QPixmap(a_bgPath));

        ui->a_SkinDescription->setText("Skin light pout neuPlayer");
    }

    if (index == 1) // Holo Fusion
    {
        ui->a_skinImage->setPixmap(QPixmap(":/Ressources/skinpreviewdark.png"));
        if(a_bgPath.isEmpty())
            ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewdark.png"));
        else
            ui->a_skinImage_2->setPixmap(QPixmap(a_bgPath));

        ui->a_SkinDescription->setText("Skin dark pour neuPlayer");
    }
    if(index == 2) //Sky Fusion
    {
        ui->a_skinImage->setPixmap(QPixmap(":/Ressources/skinpreviewcustomlight.jpg"));
        if(a_bgPath.isEmpty())
            ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewcustomwhite.jpg"));
        else
            ui->a_skinImage_2->setPixmap(QPixmap(a_bgPath));

        ui->a_SkinDescription->setText("Skin light 2 pour neuPlayer");
    }
    if(index == 3) //Night Fusion
    {
        ui->a_skinImage->setPixmap(QPixmap(":/Ressources/skinpreviewcustomdark.png"));
        if(a_bgPath.isEmpty())
            ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewcustomdark.jpg"));
        else
            ui->a_skinImage_2->setPixmap(QPixmap(a_bgPath));

        ui->a_SkinDescription->setText("Skin dark 2 pour neuPlayer");
    }
}


void Settings::changeBg()
{
    QString tempPath = QFileDialog::getOpenFileName(this, tr("S??lectionnez un fond pour le player"),QString(), tr("Images (*.jpg *.png)"));
    if(!tempPath.isEmpty())
    {
        a_bgPath = tempPath;
        ui->a_skinImage_2->setPixmap(QPixmap(a_bgPath));
    }
}


void Settings::reloadDefaultBg()
{
    a_bgPath = "";
    if (ui->a_skinPick->currentIndex() == 0) //Clean Fusion
        ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewwhite.jpg"));

    if (ui->a_skinPick->currentIndex() == 1) // Holo Fusion
        ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewdark.png"));

    if(ui->a_skinPick->currentIndex() == 2) //Sky Fusion
        ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewcustomwhite.jpg"));

    if(ui->a_skinPick->currentIndex() == 3) //Night Fusion
        ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewcustomdark.jpg"));
}

void Settings::updateOpacity(int value)
{
    a_opacityValue = value / 100.0;
    ui->a_valueSlide->setText(QString::number(value) + "%");
    a_passerelle->setOpacity(a_opacityValue);
}

void Settings::confirm()
{
        /* Library Section */
    if(a_isLibraryAtStartchecked)
    {
        if(a_isStaticLibChecked)
        {
            a_settings->setValue("Additional_Features/staticLibrary", true);
            if(a_isNewPath)
            /* Now saving playlist ! */
            setLibrary();
        }
        else if(!a_isStaticLibChecked)
        {
            a_settings->setValue("Additional_Features/staticLibrary", false);
        }
        if(a_isDynamicLibChecked)
        {
            a_settings->setValue("Additional_Features/refreshWhenNeeded", true);
            if(a_isNewPath)
                setLibrary();

            QFileInfo info (ui->a_pathView->text());
            qDebug() << info.lastModified().toMSecsSinceEpoch();
            a_settings->setValue("libModified", info.lastModified().toMSecsSinceEpoch());
        }
        else if(!a_isDynamicLibChecked)
        {
            a_settings->setValue("Additional_Features/refreshWhenNeeded", false);
        }
        if(ui->a_saveIndexActivate->isChecked())
            a_settings->setValue("Additional_Features/saveTrackIndex", true);

        else if (!ui->a_saveIndexActivate->isChecked())
            a_settings->setValue("Additional_Features/saveTrackIndex", false);
        a_settings->setValue("Additional_Features/libraryAtStartup", true);

    }
    else if (!a_isLibraryAtStartchecked) {
        a_settings->setValue("Additional_Features/libraryAtStartup", false);
    }
        /* Other Plugins */
    if(ui->a_playlistAtStartupCheck->isChecked())
        a_settings->setValue("playlistAtStartup", true);
    else
        a_settings->setValue("playlistAtStartup", false);
    if(ui->a_framelessCheck->isChecked())
        a_settings->setValue("Additional_Features/framelessWindow", true);
    else
        a_settings->setValue("Additional_Features/framelessWindow", false);

    /* Skin section */
    int currentSkin = a_settings->value("skin").toInt(); //Backup to test
    if(currentSkin != ui->a_skinPick->currentIndex() || a_settings->value("customimage").toString() != a_bgPath ) //Si un ??l??ment de skin a chang??
    {
        a_settings->setValue("skin", ui->a_skinPick->currentIndex());
        a_settings->setValue("customimage", a_bgPath);
        Skin skin(ui->a_skinPick->currentIndex(), this);
        skin.load();
        a_passerelle->loadSkin();
        a_passerelle->update();
    }
    a_settings->setValue("opacity", a_opacityValue);
    a_settings->setValue("Additional_Features/audioFade", ui->a_fadeCheck->isChecked());
    a_settings->setValue("fadeValue", a_fadeValue);
    a_passerelle->setAudioFade(ui->a_fadeCheck->isChecked());
    a_passerelle->setAudioFadeValue(a_fadeValue);
    this->cancel();
}

void Settings::cancel()
{
    a_passerelle->setOpacity(a_settings->value("opacity").toReal());
    close();
}

void Settings::setLibrary()
{
    if(!ui->a_pathView->text().isEmpty())
    {
        ui->a_confirm->setText(tr("Sauvegarde..."));
        neuPlaylist playlist(this);
        QList <QUrl> medias = playlist.setLibrary(ui->a_pathView->text());
        if(!medias.isEmpty())
            a_passerelle->updatePlaylistOfThePlayer(medias);
        ui->a_confirm->setText ("OK");
        a_settings->setValue("trackPosition", 0);
        //The Qt playlist loader will be fooled and will load the playlist smoothly as if it was saved by save();
     }
     else
     {
        a_settings->setValue("Additional_Features/staticLibrary", false);
        a_settings->setValue("Additional_Features/refreshWhenNeeded", false);
     }
}

void Settings::gotoWorkingDir()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("")); //Cross-platform solution, always works.
}

void Settings::checkUpdates()
{
    if(!a_isUpdateHandlerAlreadyCalled)
    {
        a_handler = new UpdaterHandler(nullptr);
        a_isUpdateHandlerAlreadyCalled = true;
    }
    a_handler->start("neuPlayer", QApplication::applicationVersion(), "http://sd-2.archive-host.com/membres/up/16630996856616518/version.txt", "http://sd-2.archive-host.com/membres/up/16630996856616518/neuPlayer.exe", "show");
}

void Settings::on_fadeClicked(bool enabled)
{
    ui->a_fadeSlide->setVisible(enabled); ui->a_valueFadeSlide->setVisible(enabled);
}

void Settings::updateFadeValue(int value)
{
    a_fadeValue = value * 1000;
    if(value == 1)
        ui->a_valueFadeSlide->setText(QString::number(value) + " sec");
    else
        ui->a_valueFadeSlide->setText(QString::number(value) + " secs");
}

void Settings::gotoIssues()
{
    QDesktopServices::openUrl(QUrl("https://github.com/Horoneru/neuPlayer/issues"));
}

Settings::~Settings()
{
    a_passerelle->setSettingsOpen(false);
    a_passerelle = nullptr;
    delete ui;
    delete a_settings;
    a_settings = nullptr;
}
