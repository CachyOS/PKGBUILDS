/*  This file is part of the KDE project
    Copyright (C) 2002 Alexander Neundorf <neundorf@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kmanpart.h"

#include <KAboutData>
#include <KLocalizedString>
#include <kpluginfactory.h>

#include <QUrl>

static KAboutData createAboutData()
{
   return KAboutData("kmanpart", i18n("KMan"), PROJECT_VERSION);
}

K_PLUGIN_FACTORY(KManPartFactory, registerPlugin<KManPart>();)


KManPart::KManPart(QWidget * parentWidget, QObject* parent, const QVariantList&)
: KHTMLPart(parentWidget, parent)
,m_job(nullptr)
{
   setComponentData(createAboutData());
   m_extension = new KParts::BrowserExtension(this);
}

bool KManPart::openUrl( const QUrl &url )
{
   // KHTML would detect text/plain, but we are going to write HTML to it.
   KParts::OpenUrlArguments args(arguments());
   args.setMimeType("text/html");
   setArguments(args);

   return KParts::ReadOnlyPart::openUrl(url);
}

bool KManPart::openFile()
{
   if (m_job!=nullptr)
      m_job->kill();

   begin();

   QUrl url;
   url.setScheme( "man" );
   url.setPath( localFilePath() );

   m_job = KIO::get( url, KIO::NoReload, KIO::HideProgressInfo );
   connect(m_job, &KIO::TransferJob::data, this, &KManPart::readData);
   connect(m_job, &KIO::TransferJob::result, this, &KManPart::jobDone);
   return true;
}

void KManPart::readData(KIO::Job * , const QByteArray & data)
{
   write(data,data.size());
}

void KManPart::jobDone( KJob *)
{
   m_job=nullptr;
   end();
}

#include "kmanpart.moc"

