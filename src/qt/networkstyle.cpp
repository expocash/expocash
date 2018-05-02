// Copyright (c) 2014-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "networkstyle.h"

#include "guiconstants.h"

#include <QApplication>

static const struct {
    const char *networkId;
    const char *appName;
    const int iconColorHueShift;
    const int iconColorSaturationReduction;
    const char *titleAddText;
} network_styles[] = {
    {"main", QAPP_APP_NAME_DEFAULT, 0, 0, ""},
    {"test", QAPP_APP_NAME_TESTNET, 0, 0, QT_TRANSLATE_NOOP("SplashScreen", "[testnet]")},
    {"regtest", QAPP_APP_NAME_TESTNET, 60, 1, "[regtest]"}
};
static const unsigned network_styles_count = sizeof(network_styles)/sizeof(*network_styles);

// titleAddText needs to be const char* for tr()
NetworkStyle::NetworkStyle(const QString &_appName, const int iconColorHueShift, const int iconColorSaturationReduction, const char *_titleAddText):
    appName(_appName),
    titleAddText(qApp->translate("SplashScreen", _titleAddText))
{
    // load pixmap
    QPixmap pixmap;
    QPixmap pixmap_splash;
    if (std::char_traits<char>::length(_titleAddText) == 0) {
        pixmap.load(":/icons/bitcoin");
        pixmap_splash.load(":/icons/expocash_mainnet_splash");
    } else {
        pixmap.load(":/icons/expocash_splash");
        pixmap_splash.load(":/icons/expocash_splash");
    }

    if(iconColorHueShift != 0 && iconColorSaturationReduction != 0)
    {
        // generate QImage from QPixmap
        QImage img = pixmap.toImage();
        QImage img_splash = pixmap_splash.toImage();

        int h,s,l,a,hs,ss,ls,as;

        // traverse though lines
        for(int y=0;y<img.height();y++)
        {
            QRgb *scL = reinterpret_cast< QRgb *>( img.scanLine( y ) );

            // loop through pixels
            for(int x=0;x<img.width();x++)
            {
                // preserve alpha because QColor::getHsl doesen't return the alpha value
                a = qAlpha(scL[x]);
                QColor col(scL[x]);

                // get hue value
                col.getHsl(&h,&s,&l);

                // rotate color on RGB color circle
                // 70° should end up with the typical "testnet" green
                h+=iconColorHueShift;

                // change saturation value
                if(s>iconColorSaturationReduction)
                {
                    s -= iconColorSaturationReduction;
                }
                col.setHsl(h,s,l,a);

                // set the pixel
                scL[x] = col.rgba();
            }
        }
        for(int ys=0;ys<img_splash.height();ys++)
        {
            QRgb *scLs = reinterpret_cast< QRgb *>( img_splash.scanLine( ys ) );

            // loop through pixels
            for(int xs=0;xs<img_splash.width();xs++)
            {
                // preserve alpha because QColor::getHsl doesen't return the alpha value
                as = qAlpha(scLs[xs]);
                QColor cols(scLs[xs]);

                // get hue value
                cols.getHsl(&hs,&ss,&ls);

                // rotate color on RGB color circle
                // 70° should end up with the typical "testnet" green
                hs+=iconColorHueShift;

                // change saturation value
                if(ss>iconColorSaturationReduction)
                {
                    ss -= iconColorSaturationReduction;
                }
                cols.setHsl(hs,ss,ls,as);

                // set the pixel
                scLs[xs] = cols.rgba();
            }
        }

        //convert back to QPixmap
#if QT_VERSION >= 0x040700
        pixmap.convertFromImage(img);
        pixmap_splash.convertFromImage(img_splash);
#else
        pixmap = QPixmap::fromImage(img);
        pixmap_splash = QPixmap::fromImage(img_splash);
#endif
    }

    appIcon             = QIcon(pixmap);
    splashIcon          = QIcon(pixmap_splash);
    trayAndWindowIcon   = QIcon(pixmap.scaled(QSize(256,256)));
}

const NetworkStyle *NetworkStyle::instantiate(const QString &networkId)
{
    for (unsigned x=0; x<network_styles_count; ++x)
    {
        if (networkId == network_styles[x].networkId)
        {
            return new NetworkStyle(
                    network_styles[x].appName,
                    network_styles[x].iconColorHueShift,
                    network_styles[x].iconColorSaturationReduction,
                    network_styles[x].titleAddText);
        }
    }
    return 0;
}
