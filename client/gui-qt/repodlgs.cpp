/***********************************************************************
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

// Qt
#include <QApplication>
#include <QGridLayout>
#include <QHeaderView>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>

// client
#include "repodlgs_common.h"
#include "sprite.h"

// gui-qt
#include "fc_client.h"

#include "repodlgs.h"

/************************************************************************/ /**
   Constructor for economy report
 ****************************************************************************/
eco_report::eco_report() : QWidget()
{
  QHeaderView *header;
  QGridLayout *eco_layout = new QGridLayout;
  eco_widget = new QTableWidget;
  disband_button = new QPushButton;
  sell_button = new QPushButton;
  sell_redun_button = new QPushButton;
  eco_label = new QLabel;

  QStringList slist;
  slist << _("Type") << Q_("?Building or Unit type:Name") << _("Redundant")
        << _("Count") << _("Cost") << _("U Total");
  eco_widget->setColumnCount(slist.count());
  eco_widget->setHorizontalHeaderLabels(slist);
  eco_widget->setProperty("showGrid", "false");
  eco_widget->setProperty("selectionBehavior", "SelectRows");
  eco_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  eco_widget->horizontalHeader()->resizeSections(QHeaderView::Stretch);
  eco_widget->verticalHeader()->setVisible(false);
  eco_widget->setSelectionMode(QAbstractItemView::SingleSelection);
  eco_widget->setSortingEnabled(true);
  header = eco_widget->horizontalHeader();
  header->setSectionResizeMode(1, QHeaderView::Stretch);
  header->setStretchLastSection(true);
  disband_button->setText(_("Disband"));
  disband_button->setEnabled(false);
  sell_button->setText(_("Sell All"));
  sell_button->setEnabled(false);
  sell_redun_button->setText(_("Sell Redundant"));
  sell_redun_button->setEnabled(false);
  eco_layout->addWidget(eco_widget, 1, 0, 5, 5);
  eco_layout->addWidget(disband_button, 0, 0, 1, 1);
  eco_layout->addWidget(sell_button, 0, 1, 1, 1);
  eco_layout->addWidget(sell_redun_button, 0, 2, 1, 1);
  eco_layout->addWidget(eco_label, 6, 0, 1, 5);

  connect(disband_button, &QAbstractButton::pressed, this,
          &eco_report::disband_units);
  connect(sell_button, &QAbstractButton::pressed, this,
          &eco_report::sell_buildings);
  connect(sell_redun_button, &QAbstractButton::pressed, this,
          &eco_report::sell_redundant);
  connect(eco_widget->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &,
                                  const QItemSelection &)),
          SLOT(selection_changed(const QItemSelection &,
                                 const QItemSelection &)));
  setLayout(eco_layout);
}

/************************************************************************/ /**
   Destructor for economy report
 ****************************************************************************/
eco_report::~eco_report() { gui()->remove_repo_dlg("ECO"); }

/************************************************************************/ /**
   Initializes place in tab for economy report
 ****************************************************************************/
void eco_report::init()
{
  curr_row = -1;
  gui()->gimme_place(this, "ECO");
  index = gui()->add_game_tab(this);
  gui()->game_tab_widget->setCurrentIndex(index);
}

/************************************************************************/ /**
   Refresh all widgets for economy report
 ****************************************************************************/
void eco_report::update_report()
{
  struct improvement_entry building_entries[B_LAST];
  struct unit_entry unit_entries[U_LAST];
  int entries_used, building_total, unit_total, tax, i, j;
  char buf[256];
  QTableWidgetItem *item;
  QFont f = QApplication::font();
  int h;
  QFontMetrics fm(f);
  h = fm.height() + 6;
  QPixmap *pix;
  QPixmap pix_scaled;
  struct sprite *sprite;

  eco_widget->setRowCount(0);
  eco_widget->clearContents();
  get_economy_report_data(building_entries, &entries_used, &building_total,
                          &tax);
  for (i = 0; i < entries_used; i++) {
    struct improvement_entry *pentry = building_entries + i;
    struct impr_type *pimprove = pentry->type;

    pix = NULL;
    sprite = get_building_sprite(tileset, pimprove);
    if (sprite != NULL) {
      pix = sprite->pm;
    }
    if (pix != NULL) {
      pix_scaled = pix->scaledToHeight(h);
    } else {
      pix_scaled.fill();
    }
    cid id = cid_encode_building(pimprove);

    eco_widget->insertRow(i);
    for (j = 0; j < 6; j++) {
      item = new QTableWidgetItem;
      switch (j) {
      case 0:
        item->setData(Qt::DecorationRole, pix_scaled);
        item->setData(Qt::UserRole, id);
        break;
      case 1:
        item->setTextAlignment(Qt::AlignLeft);
        item->setText(improvement_name_translation(pimprove));
        break;
      case 2:
        item->setData(Qt::DisplayRole, pentry->redundant);
        break;
      case 3:
        item->setData(Qt::DisplayRole, pentry->count);
        break;
      case 4:
        item->setData(Qt::DisplayRole, pentry->cost);
        break;
      case 5:
        item->setData(Qt::DisplayRole, pentry->total_cost);
        break;
      }
      item->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
      eco_widget->setItem(i, j, item);
    }
  }
  max_row = i;
  get_economy_report_units_data(unit_entries, &entries_used, &unit_total);
  for (i = 0; i < entries_used; i++) {
    struct unit_entry *pentry = unit_entries + i;
    struct unit_type *putype = pentry->type;
    cid id;

    pix = NULL;
    sprite = get_unittype_sprite(tileset, putype, direction8_invalid());
    if (sprite != NULL) {
      pix = sprite->pm;
    }
    id = cid_encode_unit(putype);

    eco_widget->insertRow(i + max_row);
    for (j = 0; j < 6; j++) {
      item = new QTableWidgetItem;
      item->setTextAlignment(Qt::AlignHCenter);
      switch (j) {
      case 0:
        if (pix != NULL) {
          pix_scaled = pix->scaledToHeight(h);
          item->setData(Qt::DecorationRole, pix_scaled);
        }
        item->setData(Qt::UserRole, id);
        break;
      case 1:
        item->setTextAlignment(Qt::AlignLeft);
        item->setText(utype_name_translation(putype));
        break;
      case 2:
        item->setData(Qt::DisplayRole, 0);
        break;
      case 3:
        item->setData(Qt::DisplayRole, pentry->count);
        break;
      case 4:
        item->setData(Qt::DisplayRole, pentry->cost);
        break;
      case 5:
        item->setData(Qt::DisplayRole, pentry->total_cost);
        break;
      }
      item->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
      eco_widget->setItem(max_row + i, j, item);
    }
  }
  max_row = max_row + i;
  fc_snprintf(buf, sizeof(buf), _("Income: %d    Total Costs: %d"), tax,
              building_total + unit_total);
  eco_label->setText(buf);
}

/************************************************************************/ /**
   Action for selection changed in economy report
 ****************************************************************************/
void eco_report::selection_changed(const QItemSelection &sl,
                                   const QItemSelection &ds)
{
  QTableWidgetItem *itm;
  int i;
  QVariant qvar;
  struct universal selected;
  const struct impr_type *pimprove;
  disband_button->setEnabled(false);
  sell_button->setEnabled(false);
  sell_redun_button->setEnabled(false);

  if (sl.isEmpty()) {
    return;
  }

  curr_row = sl.indexes().at(0).row();
  if (curr_row >= 0 && curr_row <= max_row) {
    itm = eco_widget->item(curr_row, 0);
    qvar = itm->data(Qt::UserRole);
    uid = qvar.toInt();
    selected = cid_decode(uid);
    switch (selected.kind) {
    case VUT_IMPROVEMENT:
      pimprove = selected.value.building;
      counter = eco_widget->item(curr_row, 3)->text().toInt();
      if (can_sell_building(pimprove)) {
        sell_button->setEnabled(true);
      }
      itm = eco_widget->item(curr_row, 2);
      i = itm->text().toInt();
      if (i > 0) {
        sell_redun_button->setEnabled(true);
      }
      break;
    case VUT_UTYPE:
      counter = eco_widget->item(curr_row, 3)->text().toInt();
      disband_button->setEnabled(true);
      break;
    default:
      log_error("Not supported type: %d.", selected.kind);
    }
  }
}

/************************************************************************/ /**
   Disband pointed units (in economy report)
 ****************************************************************************/
void eco_report::disband_units()
{
  struct universal selected;
  char buf[1024];
  hud_message_box *ask = new hud_message_box(gui()->central_wdg);
  Unit_type_id utype;

  selected = cid_decode(uid);
  utype = utype_number(selected.value.utype);
  fc_snprintf(buf, ARRAY_SIZE(buf),
              _("Do you really wish to disband every %s (%d total)?"),
              utype_name_translation(utype_by_number(utype)), counter);

  ask->set_text_title(buf, _("Disband Units"));
  ask->setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
  ask->setDefaultButton(QMessageBox::Cancel);
  ask->setAttribute(Qt::WA_DeleteOnClose);
  connect(ask, &hud_message_box::accepted, [=]() {
    struct unit_type *putype = utype_by_number(utype);
    char buf[1024];
    hud_message_box *result;

    if (putype) {
      disband_all_units(putype, false, buf, sizeof(buf));
    }

    result = new hud_message_box(gui()->central_wdg);
    result->set_text_title(buf, _("Disband Results"));
    result->setStandardButtons(QMessageBox::Ok);
    result->setAttribute(Qt::WA_DeleteOnClose);
    result->show();
  });
}

/************************************************************************/ /**
   Sell all pointed builings
 ****************************************************************************/
void eco_report::sell_buildings()
{
  struct universal selected;
  char buf[1024];
  hud_message_box *ask = new hud_message_box(gui()->central_wdg);
  const struct impr_type *pimprove;
  Impr_type_id impr_id;

  selected = cid_decode(uid);
  pimprove = selected.value.building;
  impr_id = improvement_number(pimprove);

  fc_snprintf(buf, ARRAY_SIZE(buf),
              _("Do you really wish to sell "
                "every %s (%d total)?"),
              improvement_name_translation(pimprove), counter);

  ask->set_text_title(buf, _("Sell Improvements"));
  ask->setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
  ask->setDefaultButton(QMessageBox::Cancel);
  ask->setAttribute(Qt::WA_DeleteOnClose);
  connect(ask, &hud_message_box::accepted, [=]() {
    char buf[1024];
    hud_message_box *result;
    struct impr_type *pimprove = improvement_by_number(impr_id);

    if (!pimprove) {
      return;
    }

    sell_all_improvements(pimprove, false, buf, sizeof(buf));

    result = new hud_message_box(gui()->central_wdg);
    result->set_text_title(buf, _("Sell-Off: Results"));
    result->setStandardButtons(QMessageBox::Ok);
    result->setAttribute(Qt::WA_DeleteOnClose);
    result->show();
  });
}

/************************************************************************/ /**
   Sells redundant buildings
 ****************************************************************************/
void eco_report::sell_redundant()
{
  struct universal selected;
  char buf[1024];
  QString s;
  hud_message_box *ask = new hud_message_box(gui()->central_wdg);
  const struct impr_type *pimprove;
  Impr_type_id impr_id;

  selected = cid_decode(uid);
  pimprove = selected.value.building;
  impr_id = improvement_number(pimprove);

  fc_snprintf(buf, ARRAY_SIZE(buf),
              _("Do you really wish to sell "
                "every redundant %s (%d total)?"),
              improvement_name_translation(pimprove), counter);

  ask->set_text_title(s, _("Sell Improvements"));
  ask->setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
  ask->setDefaultButton(QMessageBox::Cancel);
  ask->setAttribute(Qt::WA_DeleteOnClose);
  connect(ask, &hud_message_box::accepted, [=]() {
    char buf[1024];
    hud_message_box *result;
    struct impr_type *pimprove = improvement_by_number(impr_id);

    if (!pimprove) {
      return;
    }

    sell_all_improvements(pimprove, true, buf, sizeof(buf));

    result = new hud_message_box(gui()->central_wdg);
    result->set_text_title(buf, _("Sell-Off: Results"));
    result->setStandardButtons(QMessageBox::Ok);
    result->setAttribute(Qt::WA_DeleteOnClose);
    result->show();
  });
}

/************************************************************************/ /**
   Constructor for endgame report
 ****************************************************************************/
endgame_report::endgame_report(const struct packet_endgame_report *packet)
    : QWidget()
{
  QGridLayout *end_layout = new QGridLayout;
  end_widget = new QTableWidget;
  unsigned int i;

  players = 0;
  const size_t col_num = packet->category_num + 3;
  QStringList slist;
  slist << _("Player") << _("Nation") << _("Score");
  for (i = 0; i < col_num - 3; i++) {
    slist << Q_(packet->category_name[i]);
  }
  end_widget->setColumnCount(slist.count());
  end_widget->setHorizontalHeaderLabels(slist);
  end_widget->setProperty("showGrid", "false");
  end_widget->setProperty("selectionBehavior", "SelectRows");
  end_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  end_widget->verticalHeader()->setVisible(false);
  end_widget->setSelectionMode(QAbstractItemView::SingleSelection);
  end_widget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  end_layout->addWidget(end_widget, 1, 0, 5, 5);
  setLayout(end_layout);
}

/************************************************************************/ /**
   Destructor for endgame report
 ****************************************************************************/
endgame_report::~endgame_report() { gui()->remove_repo_dlg("END"); }

/************************************************************************/ /**
   Initializes place in tab for endgame report
 ****************************************************************************/
void endgame_report::init()
{
  gui()->gimme_place(this, "END");
  index = gui()->add_game_tab(this);
  gui()->game_tab_widget->setCurrentIndex(index);
}

/************************************************************************/ /**
   Refresh all widgets for economy report
 ****************************************************************************/
void endgame_report::update_report(
    const struct packet_endgame_player *packet)
{
  QTableWidgetItem *item;
  QPixmap *pix;
  unsigned int i;
  const struct player *pplayer = player_by_number(packet->player_id);
  const size_t col_num = packet->category_num + 3;
  end_widget->insertRow(players);
  for (i = 0; i < col_num; i++) {
    item = new QTableWidgetItem;
    switch (i) {
    case 0:
      item->setText(player_name(pplayer));
      break;
    case 1:
      pix = get_nation_flag_sprite(tileset, nation_of_player(pplayer))->pm;
      if (pix != NULL) {
        item->setData(Qt::DecorationRole, *pix);
      }
      break;
    case 2:
      item->setText(QString::number(packet->score));
      item->setTextAlignment(Qt::AlignHCenter);
      break;
    default:
      item->setText(QString::number(packet->category_score[i - 3]));
      item->setTextAlignment(Qt::AlignHCenter);
      break;
    }
    end_widget->setItem(players, i, item);
  }
  players++;
  end_widget->resizeRowsToContents();
}

/************************************************************************/ /**
   Update the economy report.
 ****************************************************************************/
void real_economy_report_dialog_update(void *unused)
{
  int i;
  eco_report *eco_rep;
  QWidget *w;

  if (gui()->is_repo_dlg_open("ECO")) {
    i = gui()->gimme_index_of("ECO");
    if (gui()->game_tab_widget->currentIndex() == i) {
      w = gui()->game_tab_widget->widget(i);
      eco_rep = reinterpret_cast<eco_report *>(w);
      eco_rep->update_report();
    }
  }
  gui()->update_sidebar_tooltips();
}

/************************************************************************/ /**
   Display the economy report.  Optionally raise it.
   Typically triggered by F5.
 ****************************************************************************/
void economy_report_dialog_popup(bool raise)
{
  int i;
  eco_report *eco_rep;
  QWidget *w;
  if (!gui()->is_repo_dlg_open("ECO")) {
    eco_rep = new eco_report;
    eco_rep->init();
    eco_rep->update_report();
  } else {
    i = gui()->gimme_index_of("ECO");
    fc_assert(i != -1);
    w = gui()->game_tab_widget->widget(i);
    if (w->isVisible()) {
      gui()->game_tab_widget->setCurrentIndex(0);
      return;
    }
    eco_rep = reinterpret_cast<eco_report *>(w);
    eco_rep->update_report();
    gui()->game_tab_widget->setCurrentWidget(eco_rep);
  }
}


/************************************************************************/ /**
   Show a dialog with player statistics at endgame.
 ****************************************************************************/
void endgame_report_dialog_start(const struct packet_endgame_report *packet)
{
  endgame_report *end_rep;
  end_rep = new endgame_report(packet);
  end_rep->init();
}

/************************************************************************/ /**
   Removes endgame report
 ****************************************************************************/
void popdown_endgame_report()
{
  int i;
  if (gui()->is_repo_dlg_open("END")) {
    i = gui()->gimme_index_of("END");
    fc_assert(i != -1);
    delete gui()->game_tab_widget->widget(i);
  }
}

/************************************************************************/ /**
   Popups endgame report to front if exists
 ****************************************************************************/
void popup_endgame_report()
{
  int i;
  if (gui()->is_repo_dlg_open("END")) {
    i = gui()->gimme_index_of("END");
    gui()->game_tab_widget->setCurrentIndex(i);
  }
}

/************************************************************************/ /**
   Received endgame report information about single player.
 ****************************************************************************/
void endgame_report_dialog_player(const struct packet_endgame_player *packet)
{
  int i;
  endgame_report *end_rep;
  QWidget *w;

  if (gui()->is_repo_dlg_open("END")) {
    i = gui()->gimme_index_of("END");
    fc_assert(i != -1);
    w = gui()->game_tab_widget->widget(i);
    end_rep = reinterpret_cast<endgame_report *>(w);
    end_rep->update_report(packet);
  }
}

/************************************************************************/ /**
   Closes economy report
 ****************************************************************************/
void popdown_economy_report()
{
  int i;
  eco_report *eco_rep;
  QWidget *w;

  if (gui()->is_repo_dlg_open("ECO")) {
    i = gui()->gimme_index_of("ECO");
    fc_assert(i != -1);
    w = gui()->game_tab_widget->widget(i);
    eco_rep = reinterpret_cast<eco_report *>(w);
    eco_rep->deleteLater();
  }
}


