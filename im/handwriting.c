#include <fcitx/frontend.h>
#include <fcitx-utils/log.h>
#include <libintl.h>
#include <fcitx/ime.h>
#include <fcitx/instance.h>
#include <fcitx/module.h>
#include <fcitx/candidate.h>
#include <dbus/dbus.h>
#include <fcitx/module/dbus/fcitx-dbus.h>

#define _(x) gettext(x)

static void* HandWritingCreate(FcitxInstance* instance);
static void HandWritingDestroy(void *arg);

boolean HandWritingInit(void *arg);
INPUT_RETURN_VALUE HandWritingDoInput(void* arg, FcitxKeySym sym,
                                      unsigned int state);
INPUT_RETURN_VALUE HandWritingGetCandWords(void *arg);
void HandWritingResetIM (void *arg);
INPUT_RETURN_VALUE HandWritingGetCandWord(void *arg,
                                          FcitxCandidateWord* candWord);
void HandWritingSave(void *arg);
void HandWritingReloadConfig(void *arg);
DBusHandlerResult HandWritingDBusFilter(DBusConnection* connection,
                                        DBusMessage* msg, void* user_data);

typedef struct _FcitxHandWriting
{
    DBusConnection* conn;
    FcitxInstance* owner;
} FcitxHandWriting;

FCITX_EXPORT_API
FcitxIMClass ime = {
    HandWritingCreate,
    HandWritingDestroy
};

FCITX_EXPORT_API
int ABI_VERSION = FCITX_ABI_VERSION;

static void*
HandWritingCreate(FcitxInstance* instance)
{
    FcitxHandWriting* handwriting = fcitx_utils_new(FcitxHandWriting);
    handwriting->owner = instance;
    handwriting->conn = FcitxDBusGetConnection(instance);
    if (handwriting->conn == NULL) {
        free(handwriting);
        return NULL;
    }

    DBusError err;
    dbus_error_init (&err);
    dbus_bus_add_match(handwriting->conn,
                       "type='signal',interface='org.fcitx.HandWriting'",
                       &err);

    dbus_connection_flush(handwriting->conn);
    if (dbus_error_is_set(&err)) {
        dbus_error_free(&err);
        FcitxLog(ERROR, "Match Error (%s)", err.message);
        free(handwriting);
        return NULL;
    }

    DBusMessage *msg = dbus_message_new_method_call(
        "org.fcitx.HandWriting",
        "/handwriting",
        "org.fcitx.HandWriting",
        "startup"
        );

    dbus_connection_send(handwriting->conn, msg, NULL);
    dbus_message_unref(msg);

    if (!dbus_connection_add_filter(handwriting->conn, HandWritingDBusFilter,
                                    handwriting, NULL)) {
        FcitxLog(ERROR, "No memory");
        free(handwriting);
        return NULL;
    }

    FcitxInstanceRegisterIM(instance,
                            handwriting,
                            "handwriting",
                            _ ( "HandWriting" ),
                            "handwriting",
                            HandWritingInit,
                            HandWritingResetIM,
                            HandWritingDoInput,
                            HandWritingGetCandWords,
                            NULL,
                            HandWritingSave,
                            HandWritingReloadConfig,
                            NULL,
                            99,
                            "zh_CN");
    dbus_error_free(&err);

    return handwriting;
}

void
HandWritingDestroy(void *arg)
{
}

boolean
HandWritingInit(void *arg)
{
    return true;
}

INPUT_RETURN_VALUE
HandWritingDoInput(void *arg, FcitxKeySym sym, unsigned int state)
{
    return IRV_TO_PROCESS;
}

INPUT_RETURN_VALUE
HandWritingGetCandWord(void *arg, FcitxCandidateWord* candWord)
{
    return IRV_TO_PROCESS;
}

INPUT_RETURN_VALUE
HandWritingGetCandWords(void *arg)
{
    return IRV_TO_PROCESS;
}

void
HandWritingReloadConfig(void *arg)
{

}

void
HandWritingResetIM(void *arg)
{

}

void
HandWritingSave(void *arg)
{

}

DBusHandlerResult
HandWritingDBusFilter(DBusConnection *connection, DBusMessage *msg,
                      void *user_data)
{
    char *s0 = NULL;
    DBusError error;
    FcitxHandWriting *handwriting = user_data;
    dbus_error_init(&error);
    if (dbus_message_is_signal(msg, "org.fcitx.HandWriting", "send_word")) {
        if (dbus_message_get_args(msg, &error, DBUS_TYPE_STRING,
                                  &s0 ,DBUS_TYPE_INVALID)) {
            FcitxInstanceCommitString(
                handwriting->owner, FcitxInstanceGetCurrentIC(
                    handwriting->owner), s0);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    dbus_error_free(&error);
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
