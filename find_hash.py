import secrets
import argparse
from hexbytes import HexBytes
from web3 import Web3

def get_sum(hb):
    s = 0
    for i in hb:
        s +=  i
    return s

parser = argparse.ArgumentParser()
parser.add_argument("guardSum", type=int)
parser.add_argument("baseHash", type=str)
args = parser.parse_args()

bh = HexBytes(args.baseHash)

lt = False
while lt == False:
    new = secrets.token_hex(32)
    res = Web3.soliditySha3(['bytes32', 'bytes32'], [bh, '0x'+new])
    s = get_sum(HexBytes(res))
    if s <= args.guardSum:
        print (new)
        print (s)
        lt = True
