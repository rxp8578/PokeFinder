/*
 * This file is part of PokéFinder
 * Copyright (C) 2017 by Admiral_Fish, bumba, and EzPzStreamz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "Wild3.hpp"
#include "ui_Wild3.h"

Wild3::Wild3(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Wild3)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setupModels();
    updateProfiles();

    qRegisterMetaType<vector<Frame3>>("vector<Frame3>");
    connect(this, SIGNAL(updateView(vector<Frame3>)), this, SLOT(updateViewSearcher(vector<Frame3>)));
}

void Wild3::changeEvent(QEvent *event)
{
    if (event != NULL)
    {
        switch (event->type())
        {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
        }
    }
}

Wild3::~Wild3()
{
    delete ui;
    delete s;
    delete g;
    delete contextMenu;
}

void Wild3::on_checkBoxDelayGenerator_clicked()
{
    if (ui->checkBoxDelayGenerator->isChecked())
    {
        ui->delayGenerator->setEnabled(true);
    }
    else
    {
        ui->delayGenerator->setEnabled(false);
        ui->delayGenerator->setText("");
    }
}

void Wild3::updateViewSearcher(vector<Frame3> frames)
{
    s->addItems(frames);
}

void Wild3::updateProfiles()
{
    profiles = Profile3::loadProfileList();

    QStandardItemModel *profile = new QStandardItemModel((int)profiles.size() + 1, 1, this);
    QStandardItem* firstProfile = new QStandardItem(tr("None"));
    profile->setItem(0, firstProfile);
    for(int i = 0; i < (int)profiles.size(); i++)
    {
        QStandardItem* item = new QStandardItem(profiles.at(i).profileName);
        profile->setItem(i + 1, item);
    }
    ui->comboBoxProfiles->setModel(profile);
}

void Wild3::setupModels()
{
    ui->tableViewGenerator->setModel(g);
    ui->tableViewGenerator->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableViewSearcher->setModel(s);
    ui->tableViewSearcher->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->initialSeedGenerator->setValues(0, 32, false);
    ui->idGenerator->setValues(0, 48, true);
    ui->sidGenerator->setValues(0, 48, true);
    ui->startingFrameGenerator->setValues(1, 32, true);
    ui->maxResultsGenerator->setValues(1, 32, true);
    ui->delayGenerator->setValues(1, 32, true);

    ui->idSearcher->setValues(0, 48, true);
    ui->sidSearcher->setValues(0, 48, true);

    ui->comboBoxMethodGenerator->setItemData(0, MethodH1);
    ui->comboBoxMethodGenerator->setItemData(1, MethodH2);
    ui->comboBoxMethodGenerator->setItemData(2, MethodH4);

    ui->comboBoxMethodSearcher->setItemData(0, MethodH1);
    ui->comboBoxMethodSearcher->setItemData(1, MethodH2);
    ui->comboBoxMethodSearcher->setItemData(2, MethodH4);

    ui->comboBoxNatureGenerator->setup();
    ui->comboBoxNatureSearcher->setup();

    ui->comboBoxHiddenPowerGenerator->setup();
    ui->comboBoxHiddenPowerSearcher->setup();

    QAction *setTargetFrame = new QAction("Set Target Frame", this);
    QAction *jumpToTarget = new QAction("Jump to Target Frame", this);
    QAction *centerTo1Second = new QAction("Center to +/- 1 Second and Set as Target Frame", this);
    QAction *centerTo2Seconds = new QAction("Center to +/- 2 Seconds and Set as Target Frame", this);
    QAction *centerTo3Seconds = new QAction("Center to +/- 3 Seconds and Set as Target Frame", this);
    QAction *centerTo5Seconds = new QAction("Center to +/- 5 Seconds and Set as Target Frame", this);
    QAction *centerTo10Seconds = new QAction("Center to +/- 10 Seconds and Set as Target Frame", this);
    QAction *centerTo1Minute = new QAction("Center to +/- 1 Minute and Set as Target Frame", this);
    QAction *outputToTxt = new QAction("Output Results to TXT", this);
    QAction *outputToCSV = new QAction("Output Results to CSV", this);

    connect(setTargetFrame, &QAction::triggered, this, &Wild3::setTargetFrameGenerator);
    connect(jumpToTarget, &QAction::triggered, this, &Wild3::jumpToTargetGenerator);
    connect(centerTo1Second, &QAction::triggered, this, [=](){
        centerFramesAndSetTargetGenerator(60);
    });
    connect(centerTo2Seconds, &QAction::triggered, this, [=](){
        centerFramesAndSetTargetGenerator(120);
    });
    connect(centerTo3Seconds, &QAction::triggered, this, [=](){
        centerFramesAndSetTargetGenerator(180);
    });
    connect(centerTo5Seconds, &QAction::triggered, this, [=](){
        centerFramesAndSetTargetGenerator(300);
    });
    connect(centerTo10Seconds, &QAction::triggered, this, [=](){
        centerFramesAndSetTargetGenerator(600);
    });
    connect(centerTo1Minute, &QAction::triggered, this, [=](){
        centerFramesAndSetTargetGenerator(3600);
    });
    connect(outputToTxt, &QAction::triggered, this, &Wild3::outputToTxt);
    connect(outputToCSV, &QAction::triggered, this, &Wild3::outputToCSV);

    contextMenu->addAction(setTargetFrame);
    contextMenu->addAction(jumpToTarget);
    contextMenu->addSeparator();
    contextMenu->addAction(centerTo1Second);
    contextMenu->addAction(centerTo2Seconds);
    contextMenu->addAction(centerTo3Seconds);
    contextMenu->addAction(centerTo5Seconds);
    contextMenu->addAction(centerTo10Seconds);
    contextMenu->addAction(centerTo1Minute);
    contextMenu->addSeparator();
    contextMenu->addAction(outputToTxt);
    contextMenu->addAction(outputToCSV);
}

void Wild3::on_saveProfileGenerator_clicked()
{
    ProfileManager3* manager = new ProfileManager3();
    manager->setAttribute(Qt::WA_QuitOnClose, false);
    connect(manager, SIGNAL(updateProfiles()), this, SLOT(refreshProfiles()));
    manager->show();
}

void Wild3::on_saveSearcher_clicked()
{
    ProfileManager3* manager = new ProfileManager3();
    manager->setAttribute(Qt::WA_QuitOnClose, false);
    connect(manager, SIGNAL(updateProfiles()), this, SLOT(refreshProfiles()));
    manager->show();
}

void Wild3::on_anyNatureGenerator_clicked()
{
    ui->comboBoxNatureGenerator->uncheckAll();
}

void Wild3::on_anyHiddenPowerGenerator_clicked()
{
    ui->comboBoxHiddenPowerGenerator->uncheckAll();
}

void Wild3::on_anyNatureSearcher_clicked()
{
    ui->comboBoxNatureSearcher->uncheckAll();
}

void Wild3::on_anyHiddenPowerSearcher_clicked()
{
    ui->comboBoxHiddenPowerSearcher->uncheckAll();
}

void Wild3::on_comboBoxProfiles_currentIndexChanged(int index)
{
    if(index == 0)
    {
        ui->idGenerator->setText("12345");
        ui->sidGenerator->setText("54321");
        ui->idSearcher->setText("12345");
        ui->sidSearcher->setText("54321");
    }
    else
    {
        ui->idGenerator->setText(QString::number(profiles.at(index - 1).tid));
        ui->sidGenerator->setText(QString::number(profiles.at(index - 1).sid));
        ui->idSearcher->setText(QString::number(profiles.at(index - 1).tid));
        ui->sidSearcher->setText(QString::number(profiles.at(index - 1).sid));
    }
}

void Wild3::refreshProfiles()
{
    emit alertProfiles(3);
}

void Wild3::on_generate_clicked()
{
    g->clear();

    u32 seed = ui->initialSeedGenerator->text().toUInt(NULL, 16);
    u32 startingFrame = ui->startingFrameGenerator->text().toUInt(NULL, 10);
    u32 maxResults = ui->maxResultsGenerator->text().toUInt(NULL, 10);
    u32 tid = ui->idGenerator->text().toUInt(NULL, 10);
    u32 sid = ui->sidGenerator->text().toUInt(NULL, 10);
    u32 offset = 0;
    if(ui->checkBoxDelayGenerator->isChecked())
        offset = ui->delayGenerator->text().toUInt(NULL, 10);

    int genderRatioIndex = ui->comboBoxGenderRatioGenerator->currentIndex();
    Generator3 generator = Generator3(maxResults, startingFrame, seed, tid, sid, offset);
    FrameCompare compare = FrameCompare(ui->ivFilterGenerator->getEvals(), ui->ivFilterGenerator->getValues(),
                                        ui->comboBoxGenderGenerator->currentIndex(), genderRatioIndex, ui->comboBoxAbilityGenerator->currentIndex(),
                                        ui->comboBoxNatureGenerator->getChecked(), ui->comboBoxHiddenPowerGenerator->getChecked(),
                                        ui->checkBoxShinyGenerator->isChecked(), ui->checkBoxDisableGenerator->isChecked());

    generator.setup((Method)ui->comboBoxMethodGenerator->currentData().toInt(NULL));

    vector<Frame3> frames = generator.generate(compare);
    g->setModel(frames);
}

void Wild3::on_search_clicked()
{
    if (isSearching == true)
        return;
    s->clear();
    s->setMethod((Method)ui->comboBoxMethodGenerator->currentData().toInt(NULL));
    std::thread job(&Wild3::search, this);
    job.detach();
}

void Wild3::search()
{
    isSearching = true;
    u32 tid = ui->idSearcher->text().toUInt(NULL, 10);
    u32 sid = ui->sidSearcher->text().toUInt(NULL, 10);

    int genderRatioIndex = ui->comboBoxGenderRatioSearcher->currentIndex();
    Searcher3 searcher = Searcher3(tid, sid, genderRatioIndex);
    FrameCompare compare = FrameCompare(ui->ivFilterSearcher->getEvals(), ui->ivFilterSearcher->getValues(), ui->comboBoxGenderSearcher->currentIndex(),
                                        genderRatioIndex, ui->comboBoxAbilitySearcher->currentIndex(), ui->comboBoxNatureSearcher->getChecked(),
                                        ui->comboBoxHiddenPowerSearcher->getChecked(), ui->checkBoxShinySearcher->isChecked(), false);

    searcher.setup((Method)ui->comboBoxMethodSearcher->currentData().toInt(NULL));

    vector<u32> min = ui->ivFilterSearcher->getLower();
    vector<u32> max = ui->ivFilterSearcher->getUpper();

    for (u32 a = min[0]; a <= max[0]; a++)
    {
        for (u32 b = min[1]; b <= max[1]; b++)
        {
            for (u32 c = min[2]; c <= max[2]; c++)
            {
                for (u32 d = min[3]; d <= max[3]; d++)
                {
                    for (u32 e = min[4]; e <= max[4]; e++)
                    {
                        for (u32 f = min[5]; f <= max[5]; f++)
                        {
                            vector<Frame3> frames = searcher.search(a, b, c, d, e, f, compare);

                            if (!frames.empty())
                                emit updateView(frames);
                        }
                    }
                }
            }
        }
    }
    isSearching = false;
}

void Wild3::centerFramesAndSetTargetGenerator(u32 centerFrames)
{
    ui->checkBoxDisableGenerator->setChecked(true);

    u32 frameNumber = ui->tableViewGenerator->model()->data(ui->tableViewGenerator->model()->index(lastIndex.row(), 0)).toString().toUInt();

    u32 startingFrame = frameNumber < centerFrames + 1U ? 1U : frameNumber - centerFrames;
    u32 selectedIndex = frameNumber < centerFrames + 1U ? frameNumber - 1U : centerFrames;
    u32 maxFrames = frameNumber < centerFrames + 1U ? frameNumber - 1U + centerFrames + 1 : centerFrames * 2 + 1;

    ui->startingFrameGenerator->setText(QString::number(startingFrame));
    ui->maxResultsGenerator->setText(QString::number(maxFrames));

    on_generate_clicked();

    targetFrame = ui->tableViewGenerator->model()->index(selectedIndex, 0);

    jumpToTargetGenerator();
}

void Wild3::setTargetFrameGenerator()
{
    targetFrame = lastIndex;
}

void Wild3::jumpToTargetGenerator()
{
    ui->tableViewGenerator->scrollTo(targetFrame, QAbstractItemView::PositionAtTop);
    ui->tableViewGenerator->selectionModel()->select(targetFrame, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
}

void Wild3::outputToTxt()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Output to TXT"), "", tr("Text File (*.txt);;All Files (*)"));

    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QString textData = "";
    int rows = g->rowCount();
    int columns = g->columnCount();

    for (int i = 0; i < columns; i++)
    {
        textData += g->headerData(i, Qt::Horizontal, 0).toString();
        if(i == 3 ||i == 13)
            textData += "\t\t";
        else
            textData += "\t";
    }

    textData += "\r\n";

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            textData += (g->data(g->index(i,j), 0).toString() != "" ? g->data(g->index(i,j), 0).toString() + "\t" : "-\t");
            if (j == 1 || (j == 13 && g->data(g->index(i,j), 0).toString().length() < 8))
            {
                textData += "\t";
            }
        }
        textData += "\r\n";             // (optional: for new line segmentation)
    }

    QTextStream out(&file);
    out << textData;
    file.close();
}

void Wild3::outputToCSV()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Output to CSV"), "", tr("CSV File (*.csv);;All Files (*)"));

    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QString textData = "";
    int rows = g->rowCount();
    int columns = g->columnCount();

    for (int i = 0; i < columns; i++)
    {
        textData += g->headerData(i, Qt::Horizontal, 0).toString();
        if (i != columns - 1)
            textData += ", ";
    }

    textData += "\n";

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            textData += g->data(g->index(i, j), 0).toString();
            if (j != columns - 1)
                textData += ", ";
        }
        textData += "\n";             // (optional: for new line segmentation)
    }

    QTextStream out(&file);
    out << textData;
    file.close();
}

void Wild3::on_tableViewGenerator_customContextMenuRequested(const QPoint &pos)
{
    if (g->rowCount() == 0)
        return;

    lastIndex = ui->tableViewGenerator->indexAt(pos);

    contextMenu->popup(ui->tableViewGenerator->viewport()->mapToGlobal(pos));
}
