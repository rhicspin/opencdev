#!/usr/bin/env python2
# vim: set sw=4 ts=4 :

import datetime
import os
import subprocess
import tempfile

os.environ["TDSVER"] = "5.0"
import Sybase
import pytz


CAD_HOST = "yellowpc"
CAD_SDDS_BASE = "/operations/app_store/RunData"
CAD_LOGREQ_BASE = "/operations/app_store/Gpm"
OUTPUT_DIR = "./"


def fetch(conn, logreq):
    print " [>] %s" % logreq
    print "  [*] Fetching metadata ..."
    cur = conn.cursor()
    logreq_full = os.path.join(CAD_LOGREQ_BASE, logreq) + ".logreq"
    #cur.execute("SELECT * FROM rhicFileHeaderV WHERE requestFile = @name", {'@name': logreq_full})
    assert(logreq_full.find("'") == -1)
    cur.execute("SELECT * FROM rhicFileHeaderV WHERE requestFile = '%s'" % logreq_full)
    res = cur.fetchall()
    if len(res) == 0:
       raise Exception("Received no metadata. Wrong logger?")
    field_list = map(lambda x: x[0], cur.description)
    fields = ", ".join(field_list)

    sqldump_path = os.path.join(OUTPUT_DIR, "sql", "%s.sql" % logreq)
    print "  [*] Dumping metadata to %s ..." % sqldump_path
    sqldump_dir = os.path.dirname(sqldump_path)
    if not os.path.isdir(sqldump_dir):
        os.makedirs(sqldump_dir)
    with open(sqldump_path, "w") as fp:
        fp.write("CREATE TABLE IF NOT EXISTS rhicFileHeaderV ("
                "processId VARCHAR(255), "
                "requestFile VARCHAR(255), "
                "orgStartTime DATETIME, "
                "timeStamp DATETIME, "
                "stopTimeStamp DATETIME, "
                "filePath VARCHAR(255), "
                "fillNo INT, "
                "stopFillNo INT, "
                "comment VARCHAR(255)"
                ");\n")
        for row in res:
            def show(val):
                if isinstance(val, datetime.datetime):
                    # First, we add TZ info to 'naive' datetime object
                    ny_val = pytz.timezone("America/New_York").localize(val)
                    # Second, convert time value to UTC
                    utc_val = ny_val.astimezone(pytz.utc)
                    return "'%s'" % utc_val.isoformat(" ")
                if val is None:
                    return "NULL"
                else:
                    return repr(val)
            values = ", ".join(map(show, row))
            query = "INSERT INTO rhicFileHeaderV (%s) VALUES (%s);" % (fields, values)
            fp.write("%s\n" % query)

    print "  [*] Copying SDDS files ..."
    with tempfile.NamedTemporaryFile(delete=True) as filelist:
        for row in res:
            remote_path = row[field_list.index("filePath")]
            # Check remote path prefix CAD_SDDS_BASE
            assert(os.path.commonprefix([remote_path, CAD_SDDS_BASE]) == CAD_SDDS_BASE)
            local_path = remote_path[len(CAD_SDDS_BASE):]
            filelist.write("%s\n" % local_path)
        filelist.flush()
        subprocess.call(["rsync", "-ptgov", "--files-from=%s" % filelist.name, "%s:%s" % (CAD_HOST, CAD_SDDS_BASE), OUTPUT_DIR])

if __name__ == '__main__':
    print " [*] Connecting to the database ..."
    conn = Sybase.connect("localhost:5000", "harmless", "harmless", "run_fy17_fill", outputmap = Sybase.DateTimeAsPython)
    fetch(conn, "Ags/Polarized_protons/CNIpolarimeter")
    fetch(conn, "RHIC/Polarimeter/Yellow/biasReadbacks")
    fetch(conn, "RHIC/Polarimeter/Blue/biasReadbacks")
    fetch(conn, "RHIC/BeamIons")
    fetch(conn, "RHIC/Rf/Voltage_Monitor_StripChart")
    fetch(conn, "RHIC/PowerSupplies/rot-ps")
    fetch(conn, "RHIC/PowerSupplies/snake-ps")
