
#include "ILastFmService.hpp"
#include "CApplication.hpp"
#include <QCryptographicHash>
#include <QDateTime>
#include <QNetworkAccessManager>

#include <QtDebug>


const QByteArray ILastFmService::m_apiKey = "20478fcc23bae9e1e2396a2b1cc52338";
const QByteArray ILastFmService::m_secret = "b2ed8ec840ec1995003bb99fb02ace44";
const QString ILastFmService::m_lastFmUrl = "http://ws.audioscrobbler.com/2.0/";


ILastFmService::ILastFmService(CApplication * application, const QByteArray& sessionKey) :
    QObject       (application),
    m_application (application),
    m_sessionKey  (sessionKey)
{
    Q_CHECK_PTR(application);

    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
}


ILastFmService::~ILastFmService()
{

}


QByteArray ILastFmService::getLastFmQuery(const QMap<QByteArray, QByteArray>& args) const
{
    QByteArray content;

    for (QMap<QByteArray, QByteArray>::const_iterator it = args.begin(); it != args.end(); ++it)
    {
        if (it != args.begin())
        {
            content.append("&");
        }

        content.append(it.key());
        content.append("=");
        content.append(encodeString(it.value()));
    }

    content.append("&api_sig=");
    content.append(getLastFmSignature(args));

    return content;
}


/**
 * Calcule la signature d'une m�thode pour envoyer une requ�te � Last.fm.
 *
 * \param args       Tableau associatif des arguments (de la forme cl� => valeur), avec la m�thode.
 * \param methodName Nom de la m�thode.
 */

QByteArray ILastFmService::getLastFmSignature(const QMap<QByteArray, QByteArray>& args) const
{
    QCryptographicHash crypto(QCryptographicHash::Md5);

    for (QMap<QByteArray, QByteArray>::const_iterator it = args.begin(); it != args.end(); ++it)
    {
        crypto.addData(it.key());
        crypto.addData(it.value());
    }

    crypto.addData(m_secret);
    return crypto.result().toHex();
}


/**
 * Log une requ�te.
 *
 * \todo Ecrire dans un fichier.
 *
 * \param url     Adresse de destination.
 * \param content Contenu du corps de la requ�te.
 */

void ILastFmService::logLastFmRequest(const QString& url, const QString& content)
{
    qDebug() << "--------------------------------------------------";
    qDebug() << QDateTime::currentDateTime();
    qDebug() << url;
    qDebug() << content;
}


/**
 * Log une r�ponse � une requ�te.
 *
 * \todo Ecrire dans un fichier.
 *
 * \param code    Code de retour de la requ�te.
 * \param content Contenu du message de retour.
 */

void ILastFmService::logLastFmResponse(int code, const QString& content)
{
    qDebug() << "--------------------------------------------------";
    qDebug() << QDateTime::currentDateTime();
    qDebug() << "Code = " << code;
    qDebug() << content;
}


/**
 * Convertit une chaine de caract�re pour l'utiliser comme URL d'une requ�te � Last.fm
 *
 * \param str Chaine � convertir.
 * \return Chaine convertie (les caract�res & et = sont remplac�s par leur code hexad�cimal).
 */

QByteArray ILastFmService::encodeString(const QByteArray& str)
{
    QByteArray res;

    // Encodage de la chaine
    for (int i = 0; i < str.size(); ++i)
    {
        if (str[i] == '&')
        {
            res.append("%26");
        }
        else if (str[i] == '=')
        {
            res.append("%3D");
        }
        else
        {
            res.append(str[i]);
        }
    }

    return res;
}