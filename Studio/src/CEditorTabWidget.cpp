#include "CEditorTabWidget.h"

#include <QMessageBox>
#include <QCloseEvent>

#include "CEditorWidget.h"

#include "Dialogs/CFileSaveDialog.h"

CEditorTabWidget::CEditorTabWidget(QWidget *parent) : QTabWidget(parent)
{
	setTabsClosable(true);
	setMovable(true);
	setContextMenuPolicy(Qt::DefaultContextMenu);
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(CurrentTabChanged(int)));
	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(TabCloseRequested(int)));
}

void CEditorTabWidget::Add(CAbstractEditor *AEditor)
{
	setCurrentIndex(addTab(AEditor->GetWidget(), "Untitled"));
	connect(
		AEditor, SIGNAL(LabelChanged(CAbstractEditor*,QString,QString)),
		this, SLOT(TabLabelChanged(CAbstractEditor*,QString,QString))
				);
}

CAbstractEditor* CEditorTabWidget::GetActiveEditor()
{
	int idx = currentIndex();
	if (idx < 0)
		return NULL;

	return widget(idx)->GetEditor();
}

void CEditorTabWidget::closeEvent(QCloseEvent *AEvent)
{
	for (int i = 0; i < count(); i++)
	{
		if (!RequestTabClose(i))
		{
			AEvent->ignore();
			return;
		}
	}

	while (count() > 0)
		CloseTab(0);

	AEvent->accept();
}

CEditorWidget* CEditorTabWidget::widget(int index)
{
	return static_cast<CEditorWidget *>(QTabWidget::widget(index));
}

bool CEditorTabWidget::RequestTabClose(int AIndex)
{
	CEditorWidget *w = widget(AIndex);
	CAbstractEditor *ed = w->GetEditor();
	if (ed->isModified())
	{
		while (true)
		{
			int result = QMessageBox::warning(this, "Confirmation",
				"File " + ed->GetFileName() + " has been modified.\n\n" +
				"Do you want to save changes?",
				QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel
			);

			if (result == QMessageBox::Cancel)
				return false;

			if (result == QMessageBox::Yes)
			{
				if (ed->isNew())
				{
					QString fn = CFileSaveDialog::Execute(ed->GetFilters());
					if (fn.isEmpty())
						continue;

					ed->SaveAs(fn);
				}
				else
					ed->Save();
			}

			break;
		}
	}

	return true;
}

void CEditorTabWidget::CloseTab(int AIndex)
{
	CEditorWidget *w = widget(AIndex);
	removeTab(AIndex);
	delete w->GetEditor();
}

void CEditorTabWidget::TabLabelChanged(CAbstractEditor *AEditor, const QString &ANewLabel, const QString &ANewToolTip)
{
	int idx = indexOf(AEditor->GetWidget());
	setTabText(idx, ANewLabel + (AEditor->isModified() ? " *" : ""));
	setTabToolTip(idx, ANewToolTip);
}

void CEditorTabWidget::CurrentTabChanged(int AIndex)
{
	if (AIndex > -1)
		widget(AIndex)->setFocus();
}

bool CEditorTabWidget::TabCloseRequested(int AIndex)
{
	if (!RequestTabClose(AIndex))
		return false;

	CloseTab(AIndex);
	return true;
}
