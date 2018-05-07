#include "classsavedata.h"
#include "ui_classsavedata.h"

#include "../cardfile.h"
#include "../classsavecardlistmodel.h"

#include <QStringListModel>

using namespace ClassEdit;

ClassSaveData::ClassSaveData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClassSaveData)
{
    ui->setupUi(this);
}

ClassSaveData::~ClassSaveData()
{
    delete ui;
}

void ClassSaveData::accept()
{
    Dictionary* dictionary = m_classData->headerDictionary();

    auto setInt = [](Dictionary *dictionary, const QString &key, const QString &text) {
        bool ok;
        int value = text.toInt(&ok);
        if (ok)
            dictionary->set(key, value);
    };

    dictionary->set("HEADER_ACADEMY", ui->textSchool->text());
    dictionary->set("HEADER_YEAR", ui->textYear->text());
    dictionary->set("HEADER_CLASS", ui->textClass->text());

    setInt(dictionary, "HEADER_HOURS", ui->textTime1->text());
    setInt(dictionary, "HEADER_MINS", ui->textTime2->text());
    setInt(dictionary, "HEADER_SECS", ui->textTime3->text());

    setInt(dictionary, "HEADER_DAYS", ui->textGameDay->text());
    dictionary->set("HEADER_GAMEWEEKDAY", ui->comboGameWeekDay->currentIndex());

    Dictionary* clubList = dictionary->value("HEADER_CLUBS").value<Dictionary*>();
    clubList->at(0).value<Dictionary*>()->set("CLUB_TYPE", ui->comboClub1->currentIndex());
    clubList->at(0).value<Dictionary*>()->set("CLUB_NAME", ui->textClub1->text());
    clubList->at(1).value<Dictionary*>()->set("CLUB_TYPE", ui->comboClub2->currentIndex());
    clubList->at(1).value<Dictionary*>()->set("CLUB_NAME", ui->textClub2->text());
    clubList->at(2).value<Dictionary*>()->set("CLUB_TYPE", ui->comboClub3->currentIndex());
    clubList->at(2).value<Dictionary*>()->set("CLUB_NAME", ui->textClub3->text());
    clubList->at(3).value<Dictionary*>()->set("CLUB_TYPE", ui->comboClub4->currentIndex());
    clubList->at(3).value<Dictionary*>()->set("CLUB_NAME", ui->textClub4->text());
    clubList->at(4).value<Dictionary*>()->set("CLUB_TYPE", ui->comboClub5->currentIndex());
    clubList->at(4).value<Dictionary*>()->set("CLUB_NAME", ui->textClub5->text());
    clubList->at(5).value<Dictionary*>()->set("CLUB_TYPE", ui->comboClub6->currentIndex());
    clubList->at(5).value<Dictionary*>()->set("CLUB_NAME", ui->textClub6->text());
    clubList->at(6).value<Dictionary*>()->set("CLUB_TYPE", ui->comboClub7->currentIndex());
    clubList->at(6).value<Dictionary*>()->set("CLUB_NAME", ui->textClub7->text());
    clubList->at(7).value<Dictionary*>()->set("CLUB_TYPE", ui->comboClub8->currentIndex());
    clubList->at(7).value<Dictionary*>()->set("CLUB_NAME", ui->textClub8->text());

    m_classData->writeHeader();

    int pcSeat = ui->comboMainCharacter->currentData().toInt();
    m_classData->footer().replace(m_classData->footer().length() - 4, 4, (const char*)&pcSeat, 4);

    m_classData->save();
    QDialog::accept();
}

void ClassSaveData::setClassData(ClassSaveCardListModel *data)
{
    m_classData = data;
    Dictionary* dictionary = m_classData->headerDictionary();
    DataEnumerable *clubEnum = dictionary->dataReader()->getDataEnumerable("CLUB_TYPE");
    DataEnumerable *weekEnum = dictionary->dataReader()->getDataEnumerable("WEEKDAY");
    static QStringListModel *clubTypeModel = new QStringListModel(QStringList()
                                                                  << clubEnum->name(0)
                                                                  << clubEnum->name(1)
                                                                  << clubEnum->name(2)
                                                                  << clubEnum->name(3)
                                                                  << clubEnum->name(4)
                                                                  << clubEnum->name(5)
                                                                  << clubEnum->name(6)
                                                                  << clubEnum->name(7)
                                                                  );
    static QStringListModel *weekModel = new QStringListModel(QStringList()
                                                                  << weekEnum->name(0)
                                                                  << weekEnum->name(1)
                                                                  << weekEnum->name(2)
                                                                  << weekEnum->name(3)
                                                                  << weekEnum->name(4)
                                                                  << weekEnum->name(5)
                                                                  << weekEnum->name(6)
                                                                  );
    Dictionary* clubList = dictionary->value("HEADER_CLUBS").value<Dictionary*>();
    auto setTextFields = [](QLineEdit *lineEdit, const QVariant &text) {
        lineEdit->setText(text.toString());
    };
    auto setClubFields = [clubTypeModel](QLineEdit *lineEdit, QComboBox *comboBox, Dictionary *club) {
        lineEdit->setText(club->value("CLUB_NAME").toString());
        comboBox->setModel(clubTypeModel);
        comboBox->setCurrentIndex(club->value("CLUB_TYPE").toInt());
    };

    setTextFields(ui->textSchool, dictionary->value("HEADER_ACADEMY"));
    setTextFields(ui->textYear, dictionary->value("HEADER_YEAR"));
    setTextFields(ui->textClass, dictionary->value("HEADER_CLASS"));

    setTextFields(ui->textTime1, dictionary->value("HEADER_HOURS"));
    setTextFields(ui->textTime2, dictionary->value("HEADER_MINS"));
    setTextFields(ui->textTime3, dictionary->value("HEADER_SECS"));

    setTextFields(ui->textGameDay, dictionary->value("HEADER_GAMEDAYS"));
    ui->comboGameWeekDay->setModel(weekModel);
    ui->comboGameWeekDay->setCurrentIndex(dictionary->value("HEADER_GAMEWEEKDAY").toInt());

    setClubFields(ui->textClub1, ui->comboClub1, clubList->at(0).value<Dictionary*>());
    setClubFields(ui->textClub2, ui->comboClub2, clubList->at(1).value<Dictionary*>());
    setClubFields(ui->textClub3, ui->comboClub3, clubList->at(2).value<Dictionary*>());
    setClubFields(ui->textClub4, ui->comboClub4, clubList->at(3).value<Dictionary*>());
    setClubFields(ui->textClub5, ui->comboClub5, clubList->at(4).value<Dictionary*>());
    setClubFields(ui->textClub6, ui->comboClub6, clubList->at(5).value<Dictionary*>());
    setClubFields(ui->textClub7, ui->comboClub7, clubList->at(6).value<Dictionary*>());
    setClubFields(ui->textClub8, ui->comboClub8, clubList->at(7).value<Dictionary*>());

    QByteArray footerEnd = m_classData->footer().right(4);
    int pcSeat = *(int*)footerEnd.data();

    QMap<int,QString> students;
    CardList &cards = m_classData->cardList();
    for (auto it = cards.begin(); it != cards.end(); it++)
        students.insert((*it)->seat(), (*it)->fullName());
    for (auto it = students.begin(); it != students.end(); it++) {
        ui->comboMainCharacter->addItem(it.value(), it.key());
        if (pcSeat == it.key()) {
            ui->comboMainCharacter->setCurrentIndex(ui->comboMainCharacter->count());
            ui->comboMainCharacter->setCurrentText(it.value());
        }
    }
}
