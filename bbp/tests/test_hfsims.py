#! /usr/bin/env python
"""
Southern California Earthquake Center Broadband Platform
Copyright 2010-2016 Southern California Earthquake Center

These are acceptance tests for the get_modelbox.py program.
$Id: test_hfsims.py 1743 2016-09-13 21:51:27Z fsilva $
"""
from __future__ import division, print_function

# Import Python modules
import os
import unittest

# Import Broadband modules
import cmp_bbp
import bband_utils
import seqnum
from install_cfg import InstallCfg
from hfsims_cfg import HfsimsCfg
from hfsims import Hfsims

class TestHfsims(unittest.TestCase):
    """
    Acceptance Test for hfsims.py
    """

    def setUp(self):
        """
        Set up and stage in all input files
        """
        self.install = InstallCfg()
        self.hfsim_cfg = HfsimsCfg()
        self.velmodel = "genslip_nr_generic1d-gp01.vmod"
        self.srcfile = "test_wh.src"
        self.srffile = "m5.89-0.20x0.20_s2379646.srf"
        self.stations = "test_stat.txt"
        self.metadata = "metadata.txt"
        self.sim_id = int(seqnum.get_seq_num())

        # Set up paths
        a_indir = os.path.join(self.install.A_IN_DATA_DIR, str(self.sim_id))
        a_tmpdir = os.path.join(self.install.A_TMP_DATA_DIR, str(self.sim_id))
        a_outdir = os.path.join(self.install.A_OUT_DATA_DIR, str(self.sim_id))
        a_logdir = os.path.join(self.install.A_OUT_LOG_DIR, str(self.sim_id))

        # Create directories
        bband_utils.mkdirs([a_indir, a_tmpdir, a_outdir, a_logdir],
                           print_cmd=False)

        cmd = "cp %s %s" % (os.path.join(self.install.A_TEST_REF_DIR,
                                         "gp", self.velmodel), a_indir)
        bband_utils.runprog(cmd, print_cmd=False)
        cmd = "cp %s %s" % (os.path.join(self.install.A_TEST_REF_DIR,
                                         "gp", self.stations), a_indir)
        bband_utils.runprog(cmd, print_cmd=False)
        cmd = "cp %s %s" % (os.path.join(self.install.A_TEST_REF_DIR,
                                         "gp", self.srffile), a_indir)
        bband_utils.runprog(cmd, print_cmd=False)
        cmd = "cp %s %s" % (os.path.join(self.install.A_TEST_REF_DIR,
                                         "gp", self.srcfile), a_indir)
        bband_utils.runprog(cmd, print_cmd=False)

    def test_hfsims(self):
        """
        Test GP HFSims code
        """
        hfs_obj = Hfsims(self.velmodel, self.srcfile, self.srffile, self.stations,
                         "LABasin", sim_id=self.sim_id)
        hfs_obj.run()
        for i in range(1, 6):
            ref_file = os.path.join(self.install.A_TEST_REF_DIR,
                                    "gp", "s%02d-hf.bbp" % (i))
            bbpfile = os.path.join(self.install.A_TMP_DATA_DIR,
                                   str(self.sim_id), "%d.s%02d-hf.bbp" %
                                   (self.sim_id, i))
            self.failIf(not cmp_bbp.cmp_bbp(bbpfile, ref_file,
                                            tolerance=0.005) == 0,
                        "output HF BBP file %s " % (bbpfile) +
                        " does not match reference hf bbp file %s" % (ref_file))

if __name__ == '__main__':
    SUITE = unittest.TestLoader().loadTestsFromTestCase(TestHfsims)
    unittest.TextTestRunner(verbosity=2).run(SUITE)
