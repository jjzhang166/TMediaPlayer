
#include "CDialogEditMetadata.hpp"
#include "CSong.hpp"
#include <QPushButton>
#include <QStandardItemModel>

// TagLib
#include <fileref.h>
#include <tag.h>
#include <flacfile.h>
#include <mpegfile.h>
#include <vorbisfile.h>
#include <tmap.h>
#include <id3v1tag.h>
#include <id3v1genres.h>
#include <id3v2tag.h>
#include <apetag.h>
#include <xiphcomment.h>
#include <commentsframe.h>
#include <unsynchronizedlyricsframe.h>
#include <textidentificationframe.h>

#include <QtDebug>


/**
 * Construit la boite de dialogue.
 *
 * \param song Morceau dont on veut afficher les métadonnées.
 */

CDialogEditMetadata::CDialogEditMetadata(CSong * song) :
    m_uiWidget (new Ui::DialogEditMetadata()),
    m_song     (song)
{
    Q_CHECK_PTR(song);

    setAttribute(Qt::WA_DeleteOnClose);
    m_uiWidget->setupUi(this);

    const QString songTitle = m_song->getTitle();
    const QString songArtist = m_song->getArtistName();

    if (songArtist.isEmpty())
    {
        setWindowTitle(tr("Metadata") + " - " + songTitle);
    }
    else
    {
        setWindowTitle(tr("Metadata") + " - " + songTitle + " - " + songArtist);
    }


    // Genres ID3v1
    TagLib::StringList genreList = TagLib::ID3v1::genreList();

    for (TagLib::StringList::ConstIterator it = genreList.begin(); it != genreList.end(); ++it)
    {
        m_uiWidget->editID3v1Genre->addItem(QString::fromUtf8(it->toCString(true)));
    }


    // Modèles
    m_modelID3v2Text = new QStandardItemModel(this);
    m_uiWidget->tableID3v2Text->setModel(m_modelID3v2Text);
    m_modelID3v2Comments = new QStandardItemModel(this);
    m_uiWidget->tableID3v2Comments->setModel(m_modelID3v2Comments);

    
    // Connexions des signaux des boutons
    QPushButton * btnSave = m_uiWidget->buttonBox->addButton(tr("Save"), QDialogButtonBox::AcceptRole);
    QPushButton * btnCancel = m_uiWidget->buttonBox->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
    QPushButton * btnApply = m_uiWidget->buttonBox->addButton(tr("Apply"), QDialogButtonBox::ApplyRole);
    QPushButton * btnReset = m_uiWidget->buttonBox->addButton(tr("Reset"), QDialogButtonBox::ResetRole);

    connect(btnSave, SIGNAL(clicked()), this, SLOT(save()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(btnApply, SIGNAL(clicked()), this, SLOT(apply()));
    connect(btnReset, SIGNAL(clicked()), this, SLOT(reset()));

    reset();
}


/**
 * Détruit la boite de dialogue.
 */

CDialogEditMetadata::~CDialogEditMetadata()
{
    delete m_uiWidget;
}


/**
 * Enregistre les modifications effectuées sur le morceau.
 *
 * \todo Implémentation.
 */

void CDialogEditMetadata::apply(void)
{
    //...
}


/**
 * Enregistre les modifications effectuées sur le morceau et ferme la boite de dialogue.
 */

void CDialogEditMetadata::save(void)
{
    apply();
    close();
}


/**
 * Recharge les métadonnées du morceau.
 *
 * \todo Implémentation.
 */

void CDialogEditMetadata::reset(void)
{
    m_modelID3v2Text->clear();
    m_modelID3v2Text->setHorizontalHeaderLabels(QStringList() << tr("Key") << tr("Value"));
    m_modelID3v2Comments->clear();
    m_modelID3v2Comments->setHorizontalHeaderLabels(QStringList() << tr("Description") << tr("Language") << tr("Comments"));

    switch (m_song->getFormat())
    {
        default:
            qWarning() << "CDialogEditMetadata::reset() : format non géré";
            return;

        case CSong::FormatMP3:
        {
            TagLib::MPEG::File file(qPrintable(m_song->getFileName()), false);

            if (!file.isValid())
            {
                qWarning() << "CDialogEditMetadata::reset() : impossible de lire le fichier MP3 " << m_song->getFileName();
                return;
            }

            initTagID3v1(file.ID3v1Tag(true));
            initTagID3v2(file.ID3v2Tag(true));
            initTagAPE(file.APETag(false));
            m_uiWidget->tabWidget->setTabEnabled(3, false);

            break;
        }

        case CSong::FormatOGG:
        {
            TagLib::Ogg::Vorbis::File file(qPrintable(m_song->getFileName()), false);

            if (!file.isValid())
            {
                qWarning() << "CDialogEditMetadata::reset() : impossible de lire le fichier Ogg " << m_song->getFileName();
                return;
            }
            
            m_uiWidget->tabWidget->setTabEnabled(0, false);
            m_uiWidget->tabWidget->setTabEnabled(1, false);
            m_uiWidget->tabWidget->setTabEnabled(2, false);
            initTagXiphComment(file.tag());

            break;
        }

        case CSong::FormatFLAC:
        {
            TagLib::FLAC::File file(qPrintable(m_song->getFileName()), false);

            if (!file.isValid())
            {
                qWarning() << "CDialogEditMetadata::reset() : impossible de lire le fichier FLAC " << m_song->getFileName();
                return;
            }
            
            initTagID3v1(file.ID3v1Tag(true));
            initTagID3v2(file.ID3v2Tag(true));
            m_uiWidget->tabWidget->setTabEnabled(2, false);
            initTagXiphComment(file.xiphComment(true));

            break;
        }
    }
}


void CDialogEditMetadata::initTagID3v1(TagLib::ID3v1::Tag * tags)
{
    if (!tags)
        return;

    m_uiWidget->editID3v1Title->setText(QString::fromUtf8(tags->title().toCString(true)));
    m_uiWidget->editID3v1Artist->setText(QString::fromUtf8(tags->artist().toCString(true)));
    m_uiWidget->editID3v1Album->setText(QString::fromUtf8(tags->album().toCString(true)));
    m_uiWidget->editID3v1Year->setValue(tags->year());
    m_uiWidget->editID3v1Comments->setText(QString::fromUtf8(tags->comment().toCString(true)));
    m_uiWidget->editID3v1Track->setValue(tags->track());
    m_uiWidget->editID3v1Genre->setCurrentIndex(TagLib::ID3v1::genreIndex(tags->genre()));
}


/// \todo Implémentation.
void CDialogEditMetadata::initTagID3v2(TagLib::ID3v2::Tag * tags)
{
    if (!tags)
        return;

    TagLib::ID3v2::FrameListMap tagMap = tags->frameListMap();

    // Liste des tags
    for (TagLib::ID3v2::FrameListMap::ConstIterator it = tagMap.begin(); it != tagMap.end(); ++it)
    {
        QString tagKey = QByteArray(it->first.data(), it->first.size());

        for (TagLib::ID3v2::FrameList::ConstIterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            TagLib::ID3v2::CommentsFrame * commentsFrame = dynamic_cast<TagLib::ID3v2::CommentsFrame *>(*it2);
            if (commentsFrame)
            {
                QList<QStandardItem *> itemList;

                itemList.append(new QStandardItem(QString::fromUtf8(commentsFrame->description().toCString(true))));
                itemList.append(new QStandardItem(CSong::getLanguageName(CSong::getLanguageForISO3Code(QByteArray(commentsFrame->language().data(), 3)))));
                itemList.append(new QStandardItem(QString::fromUtf8(commentsFrame->text().toCString(true))));

                m_modelID3v2Comments->appendRow(itemList);
            }

            TagLib::ID3v2::UnsynchronizedLyricsFrame * lyricsFrame = dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame *>(*it2);
            if (lyricsFrame)
            {
                //...
            }

            //...
        }
    }
}


/// \todo Implémentation.
void CDialogEditMetadata::initTagAPE(TagLib::APE::Tag * tags)
{
    if (!tags)
        return;

    //...
}


/// \todo Implémentation.
void CDialogEditMetadata::initTagXiphComment(TagLib::Ogg::XiphComment * tags)
{
    if (!tags)
        return;

    //...
}