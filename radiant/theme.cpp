/*
   Copyright (C) 2001-2006, William Joseph.
   All Rights Reserved.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <QStyleFactory>
#include <QApplication>
#include <QMenu>
#include <QActionGroup>

#include "preferences.h"
#include "mainframe.h"
#include "preferencesystem.h"
#include "stringio.h"


enum class ETheme{
	Light = 0,
	Dark,
	Darker
};

static QActionGroup *s_theme_group;
static ETheme s_theme = ETheme::Dark;

void theme_set( ETheme theme ){
	s_theme = theme;
#ifdef WIN32
//	QSettings settings( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat );
//	if( settings.value( "AppsUseLightTheme" ) == 0 )
#endif
	static const QPalette default_palette = qApp->palette();

	const char* sheet = R"(
	QToolTip {
		color: #ffffff;
		background-color: #4D4F4B;
		border: 1px solid white;
	}
	QScrollBar:vertical {
		width: 7px;
	}
	QScrollBar:horizontal {
		height: 7px;
	}
	QToolBar::separator:horizontal {
		width: 1px;
		margin: 3px 1px;
		background-color: #aaaaaa;
	}

	QToolBar::separator:vertical {
		height: 1px;
		margin: 1px 3px;
		background-color: #aaaaaa;
	}
	QToolButton {
	    padding: 0;
		margin: 0;
	}
	)";

	if( theme == ETheme::Light ){
		qApp->setPalette( default_palette );
	}
	else if( theme == ETheme::Dark ){
		qApp->setStyle( QStyleFactory::create( "Fusion" ) );
		QPalette darkPalette;
		QColor darkColor = QColor( 83, 84, 81 );
		QColor disabledColor = QColor( 127, 127, 127 );
		darkPalette.setColor( QPalette::Window, darkColor );
		darkPalette.setColor( QPalette::WindowText, Qt::white );
		darkPalette.setColor( QPalette::Disabled, QPalette::WindowText, disabledColor );
		darkPalette.setColor( QPalette::Base, QColor( 46, 52, 54 ) );
		darkPalette.setColor( QPalette::AlternateBase, darkColor );
		darkPalette.setColor( QPalette::ToolTipBase, Qt::white );
		darkPalette.setColor( QPalette::ToolTipText, Qt::white );
		darkPalette.setColor( QPalette::Text, Qt::white );
		darkPalette.setColor( QPalette::Disabled, QPalette::Text, disabledColor );
		darkPalette.setColor( QPalette::Button, darkColor.lighter( 130 ) );
		darkPalette.setColor( QPalette::ButtonText, Qt::white );
		darkPalette.setColor( QPalette::Disabled, QPalette::ButtonText, disabledColor.lighter( 130 ) );
		darkPalette.setColor( QPalette::BrightText, Qt::red );
		darkPalette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );

		darkPalette.setColor( QPalette::Highlight, QColor( 250, 203, 129 ) );
		darkPalette.setColor( QPalette::Inactive, QPalette::Highlight, disabledColor );
		darkPalette.setColor( QPalette::HighlightedText, Qt::black );
		darkPalette.setColor( QPalette::Disabled, QPalette::HighlightedText, disabledColor );

		qApp->setPalette( darkPalette );

		qApp->setStyleSheet( sheet );
	}
	else if( theme == ETheme::Darker ){
		qApp->setStyle( QStyleFactory::create( "Fusion" ) );
		QPalette darkPalette;
		QColor darkColor = QColor( 45, 45, 45 );
		QColor disabledColor = QColor( 127, 127, 127 );
		darkPalette.setColor( QPalette::Window, darkColor );
		darkPalette.setColor( QPalette::WindowText, Qt::white );
		darkPalette.setColor( QPalette::Base, QColor( 18, 18, 18 ) );
		darkPalette.setColor( QPalette::AlternateBase, darkColor );
		darkPalette.setColor( QPalette::ToolTipBase, Qt::white );
		darkPalette.setColor( QPalette::ToolTipText, Qt::white );
		darkPalette.setColor( QPalette::Text, Qt::white );
		darkPalette.setColor( QPalette::Disabled, QPalette::Text, disabledColor );
		darkPalette.setColor( QPalette::Button, darkColor );
		darkPalette.setColor( QPalette::ButtonText, Qt::white );
		darkPalette.setColor( QPalette::Disabled, QPalette::ButtonText, disabledColor );
		darkPalette.setColor( QPalette::BrightText, Qt::red );
		darkPalette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );

		darkPalette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );
		darkPalette.setColor( QPalette::HighlightedText, Qt::black );
		darkPalette.setColor( QPalette::Disabled, QPalette::HighlightedText, disabledColor );

		qApp->setPalette( darkPalette );

		qApp->setStyleSheet( sheet );
	}
}

void theme_contruct_menu( class QMenu *menu ){
	auto *m = menu->addMenu( "GUI Theme" );
	m->setTearOffEnabled( g_Layout_enableDetachableMenus.m_value );
	auto *group = s_theme_group = new QActionGroup( m );
	{
		auto *a = m->addAction( "Light" );
		a->setCheckable( true );
		group->addAction( a );
	}
	{
		auto *a = m->addAction( "Dark" );
		a->setCheckable( true );
		group->addAction( a );
	}
	{
		auto *a = m->addAction( "Darker" );
		a->setCheckable( true );
		group->addAction( a );
	}

	QObject::connect( s_theme_group, &QActionGroup::triggered, []( QAction *action ){
		theme_set( static_cast<ETheme>( s_theme_group->actions().indexOf( action ) ) );
	} );
}

void ThemeImport( int value ){
	s_theme = static_cast<ETheme>( value );
	if( s_theme_group != nullptr && 0 <= value && value < s_theme_group->actions().size() ){
		s_theme_group->actions().at( value )->setChecked( true );
	}
}
typedef FreeCaller1<int, ThemeImport> ThemeImportCaller;

void ThemeExport( const IntImportCallback& importer ){
	importer( static_cast<int>( s_theme ) );
}
typedef FreeCaller1<const IntImportCallback&, ThemeExport> ThemeExportCaller;


void theme_contruct(){
	GlobalPreferenceSystem().registerPreference( "GUITheme", makeIntStringImportCallback( ThemeImportCaller() ), makeIntStringExportCallback( ThemeExportCaller() ) );
	theme_set( s_theme ); // set theme here, not in importer, so it's set on the very 1st start too (when there is no preference to load)
}