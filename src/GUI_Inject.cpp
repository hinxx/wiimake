#include "GUI_Inject.h"

#include <QMessageBox>
#include "MemoryConfig.h"
#include "ISOhandler.h"
#include "CodeAssembler.h"

/* constructor */
InjectTab::InjectTab(PathInput* iso, MemConfigTab* mem, QWidget* parent) : QWidget(parent) {

    /* store iso path location and memory configuration */
    m_iso_path = iso;
    m_mem_config_tab = mem;

    /* populate the default fields of the window */
    m_source_dir = new PathInput ("C &Source Code Directory:", this);
    m_libraries.push_back(new PathInput ("&Library 1", this, false, "Static Library (*.a)"));
    m_include_dirs.push_back(new PathInput ("&Include Path 1", this));

    /* add buttons to allow for additional paths; create main button*/
    m_add_lib_button = new QPushButton("+", this);
    m_add_include_button = new QPushButton("+", this);
    m_create_button = new QPushButton("Create ISO", this);

    /* connect buttons with appropiate functions */
    connect(m_add_lib_button, SIGNAL(clicked()), this, SLOT(AddLibPath()));
    connect(m_add_include_button, SIGNAL(clicked()), this, SLOT(AddIncludeDir())); 
    connect(m_create_button, SIGNAL(clicked()), this, SLOT(CreateISO())); 

    /* create initial layout */
    m_layout = new QGridLayout(this);
    m_lib_layout = new QVBoxLayout();
    m_include_layout = new QVBoxLayout();

    /* add main button and source directory field */
    m_layout->addWidget(m_create_button, 0,0,1,-1, Qt::AlignBottom);
    m_layout->addWidget(m_source_dir, 1, 0, Qt::AlignBottom);

    /* add library section */
    m_lib_layout->addWidget(m_libraries.front());
    m_layout->addLayout(m_lib_layout, 2, 0);
    m_layout->addWidget(m_add_lib_button, 2, 1);

    /* add include path section */
    m_include_layout->addWidget(m_include_dirs.front());
    m_layout->addLayout(m_include_layout, 3, 0);
    m_layout->addWidget(m_add_include_button, 3, 1);    

    /* set layout */
    setLayout(m_layout);

}

/* add another slot for a library */
void InjectTab::AddLibPath() {

    /* insert new slot */
    std::string label = "Library " + std::to_string(m_libraries.size() + 1);
    m_libraries.push_back(new PathInput(QString::fromStdString(label), this));

    /* move button and add new library slot */
    m_lib_layout->addWidget(m_libraries.back());

}

/* add another slot for an include directory */
void InjectTab::AddIncludeDir() {

    /* insert new slot */
    std::string label = "Include Path " + std::to_string(m_include_dirs.size() + 1);
    m_include_dirs.push_back(new PathInput(QString::fromStdString(label), this));

    /* move button and add new include directory slot */
    m_include_layout->addWidget(m_include_dirs.back());

}

void InjectTab::CreateISO() {

    std::string source_dir = m_source_dir->text();
    std::vector<std::string> libs, include_dirs;

    for (auto it = m_libraries.begin(); it != m_libraries.end(); ++it) {

        if ((*it)->text().length() > 0) {

            libs.push_back((*it)->text());

        }

    }

    for (auto it = m_include_dirs.begin(); it != m_include_dirs.end(); ++it) {

        if ((*it)->text().length() > 0) {

            include_dirs.push_back((*it)->text());

        }

    }

    ISOhandler iso (m_iso_path->text());
    MemoryConfig mem_config = m_mem_config_tab->getConfig();
    CodeAssembler code (source_dir, mem_config, include_dirs, libs);
    iso.InjectCode(code.GetRawASM());
    code.CleanDirectory();

    uint32_t replace_addr = (*mem_config.begin()).first + 0x04;
    iso.IsoWrite(replace_addr, mem_config.GetInjectInstruction());

    QMessageBox mb (QMessageBox::Information, "GCI", "ISO Succesfully Created!");
    mb.exec();    

}
