#include "stdafx.h"
#include "executor.h"
#include "alfredworkflowinput.h"

AlfredWorkflowInput::AlfredWorkflowInput(const QString& workingDirectory, QObject *parent) 
    : QObject(parent)
    , workingDirectory_(workingDirectory)
{

}

bool AlfredWorkflowInput::hitKeyword(const QString& keyword)
{
    if (keyword_.isEmpty())
        return false;

    if (withSpace_)
    {
        if (keyword.length() < keyword_.length())
            return false;
        QStringList&& kws = keyword.split(' ');
        QStringList&& kws_ = keyword_.split(' ');
        if (kws.length() < kws_.length())
            return false;
        for (int index = 0; index < kws.length() && index < kws_.length(); index++)
        {
            if (kws[index] != kws_[index])
                return false;            
        }
        return true;
    }
    return keyword_ == keyword;
}

void AlfredWorkflowInput::getDerflaActions(const QString& input, DerflaActionList& derflaActions)
{
    if (!hitKeyword(input))
        return;

    DerflaActionPtr da(new DerflaAction);
    QPixmap pixmap;
    QString icon(workingDirectory_ + "/icon.png");
    if (QFile::exists(icon))
        pixmap.load(icon);
    else
        pixmap.load(":/derfla.png");
    da->setIcon(QIcon(pixmap));

    if (argumentType_ != 2)
        extractArgument(input);

    if (typeId_ == "alfred.workflow.input.keyword")
    {
        da->setTitle(text_.isEmpty() ? argument_ : embedArgument(text_));
        da->setDescription(embedArgument(subtext_));
        // do something to associate with Derfla actions 
        derflaActions.append(da);
    }
    else if (typeId_ == "alfred.workflow.input.scriptfilter")
    {
        if (runningSubtext_.isEmpty())
            da->setTitle(tr("Loading, wait for a moment please..."));
        else
            da->setTitle(embedArgument(runningSubtext_));
        da->Disabled(true);
        // run script
        Executor* executor = Executor::createExecutor(scriptExecutorType_);
        Q_ASSERT(executor);
        executor->escaping(escaping_); // must set before setting script
        executor->setScript(script_);
        executor->setWorkingDirectory(workingDirectory_);
        // do something to associate with Derfla actions 
        derflaActions.append(da);
    }
    else if (typeId_ == "alfred.workflow.input.filefilter")
    {
        da->setTitle(title_.isEmpty() ? tr("Searching files...") : embedArgument(title_));
        da->setDescription(embedArgument(subtext_));
        da->Disabled(true);
        // do search files in scope with the fields by keyword

        // do something to associate with Derfla actions 
        derflaActions.append(da);
    }
    else
    {
        Q_ASSERT(0);
        qWarning() << "unsupported workflow";
    }
}

void AlfredWorkflowInput::parse(const QString& type, const QUuid uid, const QVariantMap& v)
{
    typeId_ = type;
    uid_ = uid;
    if (v.find("escaping") != v.end())
        escaping_ = v["escaping"].toInt();
    if (v.find("argumenttype") != v.end())
        argumentType_ = v["argumenttype"].toInt();
    if (v.find("keyword") != v.end())
        keyword_ = v["keyword"].toString();
    if (v.find("title") != v.end())
        title_ = v["title"].toString();
    else if (v.find("text") != v.end())
        text_ = v["text"].toString();
    if (v.find("subtext") != v.end())
        subtext_ = v["subtext"].toString();
    if (v.find("script") != v.end())
        script_ = v["script"].toString();
    if (v.find("runningsubtext") != v.end())
        runningSubtext_ = v["runningsubtext"].toString();
    if (v.find("type") != v.end())
        scriptExecutorType_ = v["type"].toInt();
    if (v.find("withspace") != v.end())
        withSpace_ = v["withspace"].toBool();
    if (v.find("anchorfields") != v.end())
        anchorFields_ = v["anchorfields"].toBool();
    if (v.find("daterange") != v.end())
        dateRange_ = v["daterange"].toInt();
    if (v.find("includesystem") != v.end())
        includeSystem_ = v["includesystem"].toBool();
    auto scopes = v.find("scopes");
    if (scopes != v.end())
    {
        QVariantList l = scopes->toList();
        std::for_each (l.begin(), l.end(), [this](const QVariant& s) { this->scopes_.append(s.toString()); });
    }
    auto types = v.find("types");
    if (types != v.end())
    {
        QVariantList l = types->toList();
        std::for_each(l.begin(), l.end(), [this](const QVariant& t) { this->types_.append(t.toString()); });
    }
    auto fields = v.find("fields");
    if (fields != v.end())
    {
        QVariantList l = fields->toList();
        std::for_each(l.begin(), l.end(), 
            [this](const QVariant& t) { 
            AlfredWorkflowInputFieldPtr awif(new AlfredWorkflowInputField);
            QVariantMap m = t.toMap();
            awif->not_ = m["not"].toBool();
            awif->split_ = m["split"].toBool();
            awif->words_ = m["words"].toBool();
            awif->field_ = m["field"].toString();
            awif->value_ = m["value"].toString();
            this->fields_.append(awif);
        });
    }

    qDebug() << "keyword:" << keyword_;
}

void AlfredWorkflowInput::extractArgument(const QString& input)
{
    int index = input.indexOf(keyword_);
    if (index == 0)
    {
        argument_ = input.mid(keyword_.length() );
        while (argument_.startsWith(QChar(' ')))
            argument_.remove(0, 1);
    }
}

QString AlfredWorkflowInput::embedArgument(const QString& text)
{
    if (argumentType_ == 2)
        return text_;
    if (text.isEmpty())
        return argument_;

    QString res(text);
    return res.replace("{query}", argument_);
}
