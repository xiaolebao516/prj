#include "patientformdialog.h"

#include <QButtonGroup>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

#include <cmath>

namespace {

QLabel* fieldLabel(const QString& text, bool required, QWidget* parent)
{
    auto* label = new QLabel(required ? text + QStringLiteral(" <span style='color:#C4321F'>*</span>") : text,
                             parent);
    label->setProperty("role", QStringLiteral("formLabel"));
    return label;
}

bool validOptionalNumber(const QString& text)
{
    if (text.trimmed().isEmpty()) return true;
    bool ok = false;
    const double value = text.trimmed().toDouble(&ok);
    return ok && std::isfinite(value) && value > 0.0 && value <= 999.9;
}

} // namespace

PatientFormDialog::PatientFormDialog(Mode mode, QWidget* parent)
    : QDialog(parent),
      mode_(mode)
{
    setObjectName(QStringLiteral("patientFormDialog"));
    setWindowTitle(mode == Mode::Create ? QStringLiteral("新建档案") : QStringLiteral("编辑档案"));
    setModal(true);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 22);
    root->setSpacing(18);

    auto* title = new QLabel(windowTitle(), this);
    title->setProperty("role", QStringLiteral("dialogTitle"));
    root->addWidget(title);

    auto* grid = new QGridLayout;
    grid->setHorizontalSpacing(14);
    grid->setVerticalSpacing(6);
    grid->setColumnMinimumWidth(0, 84);
    grid->setColumnStretch(1, 1);
    int row = 0;

    nameEdit = new QLineEdit(this);
    nameEdit->setObjectName(QStringLiteral("patientNameEdit"));
    nameError = makeErrorLabel(this);
    grid->addWidget(fieldLabel(QStringLiteral("姓名"), true, this), row, 0);
    grid->addWidget(nameEdit, row++, 1);
    grid->addWidget(nameError, row++, 1);

    idEdit = new QLineEdit(this);
    idEdit->setObjectName(QStringLiteral("patientIdEdit"));
    idError = makeErrorLabel(this);
    grid->addWidget(fieldLabel(QStringLiteral("编号"), true, this), row, 0);
    grid->addWidget(idEdit, row++, 1);
    auto* idHint = new QLabel(mode == Mode::Create
                                  ? QStringLiteral("已自动生成，可改成自己的编号；保存后不能修改")
                                  : QStringLiteral("编号用于关联历史记录，不能修改"),
                              this);
    idHint->setProperty("role", QStringLiteral("hint"));
    grid->addWidget(idHint, row++, 1);
    grid->addWidget(idError, row++, 1);

    maleButton = new QPushButton(QStringLiteral("男"), this);
    femaleButton = new QPushButton(QStringLiteral("女"), this);
    maleButton->setObjectName(QStringLiteral("genderMaleButton"));
    femaleButton->setObjectName(QStringLiteral("genderFemaleButton"));
    genderGroup = new QButtonGroup(this);
    genderGroup->setExclusive(true);
    for (QPushButton* button : {maleButton, femaleButton}) {
        button->setCheckable(true);
        button->setProperty("variant", QStringLiteral("toggle"));
        button->setMinimumWidth(96);
        genderGroup->addButton(button);
    }
    sexError = makeErrorLabel(this);
    auto* genderRow = new QHBoxLayout;
    genderRow->setSpacing(8);
    genderRow->addWidget(maleButton);
    genderRow->addWidget(femaleButton);
    genderRow->addStretch();
    grid->addWidget(fieldLabel(QStringLiteral("性别"), true, this), row, 0);
    grid->addLayout(genderRow, row++, 1);
    grid->addWidget(sexError, row++, 1);

    yearEdit = new QLineEdit(this);
    monthEdit = new QLineEdit(this);
    dayEdit = new QLineEdit(this);
    yearEdit->setObjectName(QStringLiteral("birthYearEdit"));
    monthEdit->setObjectName(QStringLiteral("birthMonthEdit"));
    dayEdit->setObjectName(QStringLiteral("birthDayEdit"));
    yearEdit->setPlaceholderText(QStringLiteral("年"));
    monthEdit->setPlaceholderText(QStringLiteral("月"));
    dayEdit->setPlaceholderText(QStringLiteral("日"));
    yearEdit->setValidator(new QIntValidator(1900, 2999, yearEdit));
    monthEdit->setValidator(new QIntValidator(1, 12, monthEdit));
    dayEdit->setValidator(new QIntValidator(1, 31, dayEdit));
    yearEdit->setFixedWidth(84);
    monthEdit->setFixedWidth(60);
    dayEdit->setFixedWidth(60);
    for (QLineEdit* edit : {yearEdit, monthEdit, dayEdit}) {
        edit->setAlignment(Qt::AlignCenter);
        connect(edit, &QLineEdit::textChanged, this, &PatientFormDialog::updateAgePreview);
    }
    ageLabel = new QLabel(this);
    ageLabel->setObjectName(QStringLiteral("agePreview"));
    ageHint = new QLabel(this);
    ageHint->setProperty("role", QStringLiteral("hint"));
    ageHint->setWordWrap(true);
    auto* birthRow = new QHBoxLayout;
    birthRow->setSpacing(6);
    birthRow->addWidget(yearEdit);
    birthRow->addWidget(new QLabel(QStringLiteral("年"), this));
    birthRow->addWidget(monthEdit);
    birthRow->addWidget(new QLabel(QStringLiteral("月"), this));
    birthRow->addWidget(dayEdit);
    birthRow->addWidget(new QLabel(QStringLiteral("日"), this));
    birthRow->addSpacing(10);
    birthRow->addWidget(ageLabel);
    birthRow->addStretch();
    grid->addWidget(fieldLabel(QStringLiteral("出生日期"), true, this), row, 0);
    grid->addLayout(birthRow, row++, 1);
    grid->addWidget(ageHint, row++, 1);

    heightEdit = new QLineEdit(this);
    weightEdit = new QLineEdit(this);
    heightEdit->setObjectName(QStringLiteral("heightEdit"));
    weightEdit->setObjectName(QStringLiteral("weightEdit"));
    for (QLineEdit* edit : {heightEdit, weightEdit}) {
        auto* validator = new QDoubleValidator(0.1, 999.9, 1, edit);
        validator->setNotation(QDoubleValidator::StandardNotation);
        edit->setValidator(validator);
        edit->setFixedWidth(96);
    }
    bodyError = makeErrorLabel(this);
    auto* bodyRow = new QHBoxLayout;
    bodyRow->setSpacing(6);
    bodyRow->addWidget(heightEdit);
    bodyRow->addWidget(new QLabel(QStringLiteral("cm"), this));
    bodyRow->addSpacing(18);
    bodyRow->addWidget(fieldLabel(QStringLiteral("体重"), false, this));
    bodyRow->addWidget(weightEdit);
    bodyRow->addWidget(new QLabel(QStringLiteral("kg"), this));
    bodyRow->addSpacing(10);
    auto* optional = new QLabel(QStringLiteral("选填"), this);
    optional->setProperty("role", QStringLiteral("hint"));
    bodyRow->addWidget(optional);
    bodyRow->addStretch();
    grid->addWidget(fieldLabel(QStringLiteral("身高"), false, this), row, 0);
    grid->addLayout(bodyRow, row++, 1);
    grid->addWidget(bodyError, row++, 1);
    root->addLayout(grid);

    auto* buttons = new QHBoxLayout;
    buttons->addStretch();
    cancelButton = new QPushButton(QStringLiteral("取消"), this);
    cancelButton->setObjectName(QStringLiteral("patientFormCancel"));
    saveButton = new QPushButton(mode == Mode::Create ? QStringLiteral("保存") : QStringLiteral("保存修改"),
                                 this);
    saveButton->setObjectName(QStringLiteral("patientFormSave"));
    saveButton->setProperty("variant", QStringLiteral("primary"));
    saveButton->setDefault(true);
    buttons->addWidget(cancelButton);
    buttons->addWidget(saveButton);
    root->addLayout(buttons);

    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(saveButton, &QPushButton::clicked, this, &PatientFormDialog::accept);

    idEdit->setReadOnly(mode == Mode::Edit);
    updateAgePreview();
    setMinimumWidth(560);
}

QLabel* PatientFormDialog::makeErrorLabel(QWidget* parent)
{
    auto* label = new QLabel(parent);
    label->setProperty("role", QStringLiteral("error"));
    label->setWordWrap(true);
    label->hide();
    return label;
}

void PatientFormDialog::setPatient(const PatientInfo& patient)
{
    original_ = patient;
    nameEdit->setText(patient.name);
    idEdit->setText(patient.id);
    maleButton->setChecked(patient.gender == QStringLiteral("男"));
    femaleButton->setChecked(patient.gender == QStringLiteral("女"));
    const QDate birth = QDate::fromString(patient.birthDay, QStringLiteral("yyyy-MM-dd"));
    if (birth.isValid()) {
        yearEdit->setText(QString::number(birth.year()));
        monthEdit->setText(QString::number(birth.month()));
        dayEdit->setText(QString::number(birth.day()));
    }
    heightEdit->setText(patient.height);
    weightEdit->setText(patient.weight);
    updateAgePreview();
}

void PatientFormDialog::setSuggestedId(const QString& id)
{
    if (mode_ == Mode::Create) idEdit->setText(id);
}

void PatientFormDialog::setExistingIds(const QSet<QString>& ids)
{
    existingIds_ = ids;
}

void PatientFormDialog::setSaveButtonText(const QString& text)
{
    saveButton->setText(text);
}

QDate PatientFormDialog::enteredBirthDate() const
{
    bool yearOk = false;
    bool monthOk = false;
    bool dayOk = false;
    const int year = yearEdit->text().trimmed().toInt(&yearOk);
    const int month = monthEdit->text().trimmed().toInt(&monthOk);
    const int day = dayEdit->text().trimmed().toInt(&dayOk);
    if (!yearOk || !monthOk || !dayOk) return QDate();
    return QDate(year, month, day);
}

int PatientFormDialog::ageOn(const QDate& birthDate, const QDate& onDate)
{
    if (!birthDate.isValid() || !onDate.isValid() || birthDate > onDate) return -1;
    int age = onDate.year() - birthDate.year();
    if (onDate.month() < birthDate.month() ||
        (onDate.month() == birthDate.month() && onDate.day() < birthDate.day())) {
        --age;
    }
    return age;
}

QString PatientFormDialog::suggestId(const QList<PatientInfo>& patients, const QDate& today)
{
    const QString prefix = QStringLiteral("P%1-").arg(today.toString(QStringLiteral("yyyyMMdd")));
    QSet<QString> used;
    for (const PatientInfo& patient : patients) used.insert(patient.id);
    for (int sequence = 1; sequence < 10000; ++sequence) {
        const QString candidate = prefix + QStringLiteral("%1").arg(sequence, 3, 10, QLatin1Char('0'));
        if (!used.contains(candidate)) return candidate;
    }
    return prefix + QStringLiteral("X");
}

void PatientFormDialog::updateAgePreview()
{
    const QDate birth = enteredBirthDate();
    const QDate today = QDate::currentDate();
    const bool typed = !(yearEdit->text().isEmpty() && monthEdit->text().isEmpty() && dayEdit->text().isEmpty());
    const int age = ageOn(birth, today);
    ageLabel->setProperty("state", QString());
    ageHint->setProperty("role", QStringLiteral("hint"));
    if (birth.isValid() && birth.year() >= 1900 && age >= 0) {
        ageLabel->setText(QStringLiteral("年龄 %1 岁").arg(age));
        ageLabel->setProperty("state", age < 20 ? QStringLiteral("warn") : QStringLiteral("ok"));
        if (age < 20) {
            ageHint->setText(QStringLiteral("未满 20 岁，将使用儿童/青少年参考曲线，请确认年份没有填错。"));
            ageHint->setProperty("role", QStringLiteral("warn"));
        } else {
            ageHint->setText(QStringLiteral("请核对年龄是否正确。"));
        }
    } else {
        ageLabel->setText(QStringLiteral("年龄 --"));
        if (typed) {
            ageHint->setText(QStringLiteral("日期不完整、无效或晚于今天，请检查年、月、日。"));
            ageHint->setProperty("role", QStringLiteral("error"));
        } else {
            ageHint->setText(QStringLiteral("例如 1958 年 4 月 12 日。填好后会自动算出年龄。"));
        }
    }
    for (QLabel* label : {ageLabel, ageHint}) {
        label->style()->unpolish(label);
        label->style()->polish(label);
    }
}

PatientInfo PatientFormDialog::patient() const
{
    PatientInfo patient = original_;
    patient.name = nameEdit->text().trimmed();
    patient.id = mode_ == Mode::Edit ? original_.id : idEdit->text().trimmed();
    if (maleButton->isChecked()) patient.gender = QStringLiteral("男");
    else if (femaleButton->isChecked()) patient.gender = QStringLiteral("女");
    else patient.gender.clear();
    const QDate birth = enteredBirthDate();
    patient.birthDay = birth.isValid() ? birth.toString(QStringLiteral("yyyy-MM-dd")) : QString();
    patient.height = heightEdit->text().trimmed();
    patient.weight = weightEdit->text().trimmed();
    return patient;
}

bool PatientFormDialog::validate()
{
    updateAgePreview();
    bool ok = true;
    const auto show = [&ok](QLabel* label, const QString& message) {
        label->setText(message);
        label->setVisible(!message.isEmpty());
        if (!message.isEmpty()) ok = false;
    };

    show(nameError, nameEdit->text().trimmed().isEmpty() ? QStringLiteral("请填写姓名") : QString());

    QString idMessage;
    const QString id = idEdit->text().trimmed();
    if (id.isEmpty()) idMessage = QStringLiteral("请填写编号");
    else if (mode_ == Mode::Create && existingIds_.contains(id))
        idMessage = QStringLiteral("该编号已存在，请从档案中选择，或换一个编号");
    show(idError, idMessage);

    show(sexError, (!maleButton->isChecked() && !femaleButton->isChecked())
                       ? QStringLiteral("请选择性别")
                       : QString());

    const QDate birth = enteredBirthDate();
    const bool birthOk = birth.isValid() && birth.year() >= 1900 && birth <= QDate::currentDate();
    if (!birthOk) {
        ageHint->setText(QStringLiteral("请填写有效的出生日期，不能晚于今天。"));
        ageHint->setProperty("role", QStringLiteral("error"));
        ageHint->style()->unpolish(ageHint);
        ageHint->style()->polish(ageHint);
        ok = false;
    }

    QString bodyMessage;
    if (!validOptionalNumber(heightEdit->text()))
        bodyMessage = QStringLiteral("身高请填写大于 0 且不超过 999.9 的数字，也可以留空。");
    else if (!validOptionalNumber(weightEdit->text()))
        bodyMessage = QStringLiteral("体重请填写大于 0 且不超过 999.9 的数字，也可以留空。");
    show(bodyError, bodyMessage);

    return ok;
}

void PatientFormDialog::accept()
{
    if (!validate()) return;
    QDialog::accept();
}
