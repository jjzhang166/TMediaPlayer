
#include "CDialogEditStaticPlayList.hpp"
#include "CStaticPlayList.hpp"
#include "CApplication.hpp"
#include <QStandardItemModel>
#include <QMessageBox>
#include <QPushButton>

#include <QtDebug>


CDialogEditStaticPlayList::CDialogEditStaticPlayList(CStaticPlayList * playList, CApplication * application, CListFolder * folder, const QList<CSong *>& songs) :
    QDialog       (application),
    m_uiWidget    (new Ui::DialogEditStaticPlayList()),
    m_playList    (playList),
    m_application (application),
    m_folder      (folder),
    m_songs       (songs)
{
    Q_CHECK_PTR(application);

    setAttribute(Qt::WA_DeleteOnClose);
    m_uiWidget->setupUi(this);

    if (!m_playList)
    {
        m_playList = new CStaticPlayList(m_application);
    }

    m_uiWidget->editName->setText(m_playList->getName());


    // Connexions des signaux des boutons
    QPushButton * btnSave = m_uiWidget->buttonBox->addButton(tr("Save"), QDialogButtonBox::AcceptRole);
    QPushButton * btnCancel = m_uiWidget->buttonBox->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);

    connect(btnSave, SIGNAL(clicked()), this, SLOT(save()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}


/**
 * Détruit le widget.
 */

CDialogEditStaticPlayList::~CDialogEditStaticPlayList()
{
    //qDebug() << "CDialogEditStaticPlayList::~CDialogEditStaticPlayList()";
    delete m_uiWidget;
}


/**
 * Enregistre les modifications.
 *
 * \todo Ajouter les morceaux à la liste.
 */

void CDialogEditStaticPlayList::save(void)
{
    QString name = m_uiWidget->editName->text();

    if (name.isEmpty())
    {
        QMessageBox::warning(this, QString(), tr("You need to choose a name for the playlist."));
        return;
    }

    m_playList->setName(name);
    m_playList->setFolder(m_folder);

    if (!m_playList->updateDatabase())
    {
        return;
    }

    m_application->addPlayList(m_playList);

    // Ajout des morceaux à la liste
    m_playList->addSongs(m_songs, false);

    close();
}