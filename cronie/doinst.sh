config() {
    NEW="$1"
    OLD="$(dirname $NEW)/$(basename $NEW .new)"
    # If there's no config file by that name, mv it over:
    if [ ! -r $OLD ]; then
        mv $NEW $OLD
    elif [ "$(cat $OLD | md5sum)" = "$(cat $NEW | md5sum)" ]; then
        # toss the redundant copy
        rm $NEW
    fi
    # Otherwise, we leave the .new copy for the admin to consider...
}

perms() {
    # Keep same perms on file
    NEW="$1"
    OLD="$(dirname $NEW)/$(basename $NEW .new)"
    if [ -e $OLD ]; then
        cp -a $OLD $NEW.incoming
        cat $NEW >$NEW.incoming
        mv $NEW.incoming $NEW
    fi
    config $NEW
}

perms etc/rc.d/rc.crond.new
config var/spool/cron/root.new
config etc/default/crond.new
config etc/default/anacron.new
config etc/cron.deny.new
config etc/crontab.new
