#include "petdisplay.h"
#include "ui_petdisplay.h"
#include "petliked.h"
#include "petownerhelp.h"
#include "createaccount.h"
#include "mainwindow.h"

PetDisplay::PetDisplay(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PetDisplay)
{
    ui->setupUi(this);

    QString photoFilePath(":/resources/imgs/petPhoto0.jpg");
    petPic.load(photoFilePath);
    int width = ui->animalDisplay->width();
    int height = ui->animalDisplay->height();
    ui->animalDisplay->setPixmap(petPic.scaled(width, height, Qt::KeepAspectRatio));

    baseQuery = "select * from pet ";
    // adding icons to the page
    QPixmap icon;

    QString icon0(":/icons/icons/Name.png");
    icon.load(icon0);
    width = ui->icon0_display->width();
    height = ui->icon0_display->height();
    ui->icon0_display->setPixmap(icon.scaled(width, height, Qt::KeepAspectRatio));

    QString icon1(":/icons/icons/Type.png");
    icon.load(icon1);
    width = ui->icon1_display->width();
    height = ui->icon1_display->height();
    ui->icon1_display->setPixmap(icon.scaled(width, height, Qt::KeepAspectRatio));

    QString icon2(":/icons/icons/Breed.png");
    icon.load(icon2);
    width = ui->icon2_display->width();
    height = ui->icon2_display->height();
    ui->icon2_display->setPixmap(icon.scaled(width, height, Qt::KeepAspectRatio));

    QString icon3(":/icons/icons/Age.png");
    icon.load(icon3);
    width = ui->icon3_display->width();
    height = ui->icon3_display->height();
    ui->icon3_display->setPixmap(icon.scaled(width, height, Qt::KeepAspectRatio));

    QString icon4(":/icons/icons/Location.png");
    icon.load(icon4);
    width = ui->icon4_display->width();
    height = ui->icon4_display->height();
    ui->icon4_display->setPixmap(icon.scaled(width, height, Qt::KeepAspectRatio));

    QString icon5(":/icons/icons/Shelter.png");
    icon.load(icon5);
    width = ui->icon5_display->width();
    height = ui->icon5_display->height();
    ui->icon5_display->setPixmap(icon.scaled(width, height, Qt::KeepAspectRatio));

    currentPos = 0;
    fetchPets();
    getCurrentUser();

    ui->progressBar->setOrientation(Qt::Horizontal);
    ui->progressBar->setRange(1, pets.size());
    ui->progressBar->setValue(currentPos+1);
}

PetDisplay::~PetDisplay()
{
    delete ui;
}

// changes available breeds based on pet type
void PetDisplay::on_typeBox_activated(const QString &arg1)
{

    if (arg1 == "Dog") {
        // clearing all of the drop down menus
        ui->breedBox->clear();
        ui->colorBox->clear();
        ui->hairLenBox->clear();

        QSqlQuery query;
        query.prepare("select distinct breed from pet where is_cat = 0");

        if (query.exec()) {
            while (query.next()) {
                QString breed = query.value(0).toString();
                ui->breedBox->addItem(breed);
            }
        } else {
            qDebug() << "Error getting dog breeds:" << query.lastError().text();
        }

        if (prefString.isEmpty()) {
            prefString.remove("where is_cat = 1 ");
            prefString.append("where is_cat = 0 ");
        } else {
            prefString.remove("and is_cat = 1 ");
            prefString.append("and is_cat = 0 ");
        }
    }

    if (arg1 == "Cat") {
        // clearing all of the drop down menus
        ui->breedBox->clear();
        ui->colorBox->clear();
        ui->hairLenBox->clear();

        QSqlQuery query;
        query.prepare("select distinct breed from pet where is_cat = 1");

        if (query.exec()) {
            while (query.next()) {
                QString breed = query.value(0).toString();
                ui->breedBox->addItem(breed);
            }
        } else {
            qDebug() << "Error getting cat breeds:" << query.lastError().text();
        }

        if (prefString.isEmpty()) {
            prefString.remove("where is_cat = 0 ");
            prefString.append("where is_cat = 1 ");
        } else {
            prefString.remove("and is_cat = 0 ");
            prefString.append("and is_cat = 1 ");
        }
    }
}

void PetDisplay::on_breedBox_activated(const QString &arg1)
{
    if (prefString.isEmpty()) {
        QString queryString = "where breed = ";
        queryString.append('\'');
        queryString.append(arg1);
        queryString.append('\'');
        queryString.append(" ");
        prefString.append(queryString);
    } else {
        QString queryString = "and breed = ";
        int stringIndex = prefString.indexOf(queryString);

        if (stringIndex == -1) {
            queryString.append('\'');
            queryString.append(arg1);
            queryString.append('\'');
            queryString.append(" ");
            prefString.append(queryString);
        } else {
            // TODO: figure out how to remove the existing breed string.
        }
    }

    QSqlQuery query;
    query.prepare("select distinct color from pet where breed = ?");
    query.addBindValue(arg1);

    if (query.exec()) {
        ui->colorBox->clear();
        ui->hairLenBox->clear();

        while (query.next()) {
            QString color = query.value(0).toString();
            ui->colorBox->addItem(color);
        }
    } else {
        qDebug() << "Error getting breed colors:" << query.lastError().text();
    }
}

void PetDisplay::on_pushButton_clicked()
{
    fetchPets();
}

void PetDisplay::displayPet(Pet p)
{
    int numberOfPhotos = 8;

    srand(time(0));
    int photoNum = rand() % numberOfPhotos;

    QString photoString = QString::number(photoNum);

    QString tempPath(":/resources/imgs/petPhoto");
    tempPath.append(photoString);
    QString filePath = tempPath.append(".jpg");

    petPic.load(filePath);
    int width = ui->animalDisplay->width();
    int height = ui->animalDisplay->height();
    ui->animalDisplay->setPixmap(petPic.scaled(width, height, Qt::KeepAspectRatio));

    ui->label_name->setText(p.getName());
    ui->label_breed->setText(p.getBreed());
    ui->label_age->setText(QString::number(p.getAge()));

    if (!p.getIs_cat())
        ui->label_type->setText("Dog");
    else
        ui->label_type->setText("Cat");

    QSqlQuery query;
    query.prepare("select shelter_id from pet where pet_id = ?");
    query.addBindValue(p.getPet_id());

    // This does not update the label text when the dislike button is selected.
    if (query.exec()) {
        if (query.next()) {
            int shelterID = query.value(0).toInt();

            Shelter s(shelterID);
            ui->label_shelter_name->setText(s.getName());
            ui->label_location->setText(s.getLocation());
        }
    } else {
        qDebug() << "Error getting pet's shelter:" << query.lastError().text();
    }
}

void PetDisplay::getCurrentUser()
{
    QString name = currentUser.getFirstName();
    ui->label_user_name->setText("Welcome " + name + "!");

    QSqlQuery query;
    query.prepare("select count(pet_id) from Liked_By where adopter_id = ?");
    query.addBindValue(currentUser.getID());

    if (query.exec()) {
        if (query.next()) {
            int numLiked = query.value(0).toInt();

            if (numLiked == 1)
                ui->statusbar->showMessage("You have liked 1 pet.");
            else
                ui->statusbar->showMessage("You have liked " + QString::number(numLiked) + " pets.");
        }
    } else {
        qDebug() << "Error getting number of pets liked:" << query.lastError().text();
    }
}

void PetDisplay::on_profileButton_clicked()
{
   profileUI = new PetProfile(this);
   profileUI->setPDisplay(pets.at(currentPos));
   profileUI->setModal(true);
   profileUI->exec();
}

void PetDisplay::on_button_like_clicked()
{
    Pet currPet = pets.at(currentPos);

    QSqlQuery query;
    query.prepare("insert into Liked_By (adopter_id, pet_id)"
                  "values (?, ?)");
    query.addBindValue(currentUser.getID());
    query.addBindValue(currPet.getPet_id());

    if (query.exec()) {
        if (currentPos + 1 > pets.size() - 1) {
            QMessageBox::warning(this, "No More Pets!", "You've successfully liked this pet, but no more pets can be found. Please try expanding your search to find more pets.");
        } else {
            currentPos++;
            displayPet(pets.at(currentPos));
        }
    } else {
        qDebug() << "Error liking pet:" << query.lastError().text();
    }

    updateBar();
}

void PetDisplay::on_button_dislike_clicked()
{
    if (currentPos + 1 > pets.size() - 1) {
        QMessageBox::critical(this, "No More Pets!", "No more pets could be found! Please try expanding your search to find more pets.");
    } else {
        currentPos++;
        displayPet(pets.at(currentPos));
    }

    updateBar();
}

void PetDisplay::fetchPets()
{
    qDebug() << "Running query " + baseQuery + prefString;

    QSqlQuery query;
    if (query.exec(baseQuery + prefString)) {
        if (!pets.empty())
            pets.clear();

        while (query.next()) {
            int pID = query.value(0).toInt();

            Pet p(pID);
            pets.push_back(p);
        }

        if (!pets.empty()) {
            currentPos = 0;
            displayPet(pets.front());
        }
        else
            QMessageBox::critical(this, "No Pets Found", "No pets could be found with your search parameters. Please change your search and try again.");
    } else {
        qDebug() << "Error fetching pets:" << query.lastError().text();
    }
}

void PetDisplay::on_hypoBox_activated(const QString &arg1)
{
    if (arg1 == "Yes") {
        if (prefString.isEmpty()) {
            prefString.remove("where hypoallergenic = 0 ");
            prefString.append("where hypoallergenic = 1 ");
        } else {
            prefString.remove("and hypoallergenic = 0 ");
            prefString.append("and hypoallergenic = 1 ");
        }

    } else if (arg1 == "No") {
        if (prefString.isEmpty()) {
            prefString.remove("where hypoallergenic = 1 ");
            prefString.append("where hypoallergenic = 0 ");
        } else {
            prefString.remove("and hypoallergenic = 1 ");
            prefString.append("and hypoallergenic = 0 ");
        }
    }
}

void PetDisplay::on_actionLog_out_triggered()
{
    hide();
    currentUser.logOut();
    parentWidget()->show();
}

void PetDisplay::on_actionLiked_triggered()
{
    hide();
    PetLiked *likedUI = new PetLiked(this);
    likedUI->setAttribute(Qt::WA_DeleteOnClose);
    likedUI->show();
}

void PetDisplay::on_actionHelp_triggered()
{
    hide();
    petownerhelp *helpUI = new petownerhelp(this);
    helpUI->setAttribute(Qt::WA_DeleteOnClose);
    helpUI->show();
}

void PetDisplay::on_dislikeBoxType_clicked(bool checked)
{

}

void PetDisplay::on_dislikeBoxBreed_clicked(bool checked)
{

}

void PetDisplay::on_dislikeBoxColor_clicked(bool checked)
{

}

void PetDisplay::on_dislikeBoxHairLen_clicked(bool checked)
{

}

void PetDisplay::updateBar()
{
    ui->progressBar->setValue(currentPos+1);
}

void PetDisplay::on_minAgeSlider_valueChanged(int value)
{
    QString labelText = "Minimum Age: ";
    QString numText = QString::number(value);
    labelText.append(numText);
    ui->label_search_minage->setText(labelText);
}

void PetDisplay::on_maxAgeSlider_valueChanged(int value)
{
    QString labelText = "Maximum Age: ";
    QString numText = QString::number(value);
    labelText.append(numText);
    ui->label_search_maxage->setText(labelText);
}

void PetDisplay::on_minWeightSlider_valueChanged(int value)
{
    QString labelText = "Minimum Weight: ";
    QString numText = QString::number(value);
    labelText.append(numText);
    ui->label_search_minweight->setText(labelText);
}

void PetDisplay::on_maxWeightSlider_valueChanged(int value)
{
    QString labelText = "Maximum Weight: ";
    QString numText = QString::number(value);
    labelText.append(numText);
    ui->label_search_maxweight->setText(labelText);
}

void PetDisplay::on_searchRangeSlider_valueChanged(int value)
{
    QString labelText = "Search Range: ";
    QString numText = QString::number(value);
    labelText.append(numText);
    ui->label_search_range->setText(labelText);
}

void PetDisplay::on_minAgeSlider_sliderReleased()
{
    int value = ui->minAgeSlider->value();

    if (prefString.isEmpty()) {
        prefString.append("where age >= " + QString::number(value) + " ");
    } else {
        QString queryString = "and age >= ";
        int index = prefString.indexOf(queryString);

        if (index == -1) {
            prefString.append("and age >= " + QString::number(value) + " ");
        } else {

        }
    }
}

void PetDisplay::on_maxAgeSlider_sliderReleased()
{
    int value = ui->maxAgeSlider->value();

    if (prefString.isEmpty()) {
        prefString.append("where age <= " + QString::number(value) + " ");
    } else {
        QString queryString = "and age <= ";
        int index = prefString.indexOf(queryString);

        if (index == -1) {
            prefString.append("and age <= " + QString::number(value) + " ");
        } else {

        }
    }
}

void PetDisplay::on_minWeightSlider_sliderReleased()
{
    int value = ui->minWeightSlider->value();

    if (prefString.isEmpty()) {
        prefString.append("where weight >= " + QString::number(value) + " ");
    } else {
        QString queryString = "and weight >= ";
        int index = prefString.indexOf(queryString);

        if (index == -1) {
            prefString.append("and weight >= " + QString::number(value) + " ");
        } else {

        }
    }
}

void PetDisplay::on_maxWeightSlider_sliderReleased()
{
    int value = ui->maxWeightSlider->value();

    if (prefString.isEmpty()) {
        prefString.append("where weight <= " + QString::number(value) + " ");
    } else {
        QString queryString = "and weight <= ";
        int index = prefString.indexOf(queryString);

        if (index == -1) {
            prefString.append("and weight <= " + QString::number(value) + " ");
        } else {

        }
    }
}

void PetDisplay::on_actionQuit_triggered()
{
   QApplication::quit();
}

void PetDisplay::on_colorBox_activated(const QString &arg1)
{
    if (prefString.isEmpty()) {
        QString queryString = "where color = ";
        queryString.append('\'');
        queryString.append(arg1);
        queryString.append('\'');
        prefString.append(queryString);
    } else {
        QString queryString = "and color = ";
        int index = prefString.indexOf(queryString);

        if (index == -1) {
            queryString.append('\'');
            queryString.append(arg1);
            queryString.append('\'');
            prefString.append(queryString);
        } else {

        }
    }
}

void PetDisplay::on_hairLenBox_activated(const QString &arg1)
{
    if (prefString.isEmpty()) {
        QString queryString = "where hair_length = ";
        queryString.append('\'');
        queryString.append(arg1);
        queryString.append('\'');
        prefString.append(queryString);
    } else {
        QString queryString = "and hair_length = ";
        int index = prefString.indexOf(queryString);

        if (index == -1) {
            queryString.append('\'');
            queryString.append(arg1);
            queryString.append('\'');
            prefString.append(queryString);
        } else {

        }
    }
}

void PetDisplay::on_actionAbout_BreadnButter_triggered()
{
    QMessageBox::about(this, "About BreadnButter", "Welcome to BreadnButter!\n "
                   "This applications allows quick, efficient and effective services for those looking for pets!\n"
                   "If you are looking for people to adopt your pets, please don't hesistate to make a shelter account!");
}
