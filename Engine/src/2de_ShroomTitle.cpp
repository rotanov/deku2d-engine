#include "2de_ImageUtils.h"

unsigned IMAGE_SHROOM_TITLE_SIZE = 1477;
char IMAGE_SHROOM_TITLE_DATA[] = {'\x89','\x50','\x4e','\x47','\xd','\xa','\x1a','\xa','\x0','\x0','\x0','\xd','\x49','\x48','\x44','\x52','\x0','\x0','\x1','\x0','\x0','\x0','\x1','\x0','\x4','\x3','\x0','\x0','\x0','\xae','\x5c','\xb5','\x55','\x0','\x0','\x0','\x30','\x50','\x4c','\x54','\x45','\x4','\x2','\x4','\x88','\x88','\x88','\xc8','\xc9','\xc8','\x48','\x48','\x48','\x29','\x28','\x29','\xe8','\xe8','\xe8','\xa8','\xa9','\xa8','\x6a','\x6a','\x6a','\x18','\x18','\x18','\x98','\x97','\x98','\x59','\x58','\x59','\x38','\x37','\x38','\xfc','\xfe','\xfc','\xb8','\xba','\xb8','\xd8','\xd9','\xd8','\x78','\x77','\x78','\xf4','\x7','\x30','\x43','\x0','\x0','\x0','\x1','\x74','\x52','\x4e','\x53','\x0','\x40','\xe6','\xd8','\x66','\x0','\x0','\x0','\x9','\x70','\x48','\x59','\x73','\x0','\x0','\xb','\x12','\x0','\x0','\xb','\x12','\x1','\xd2','\xdd','\x7e','\xfc','\x0','\x0','\x5','\x2e','\x49','\x44','\x41','\x54','\x78','\x9c','\xed','\x9c','\x4d','\x68','\x5c','\x55','\x14','\xc7','\x9f','\x76','\xb4','\xcf','\x4','\x3f','\x82','\xab','\x82','\xa8','\x8','\x2e','\x84','\x54','\xa5','\x71','\xd1','\x2f','\xe4','\x81','\x15','\x91','\x16','\x17','\xea','\xe0','\xc2','\x38','\x8','\xda','\x40','\x21','\x15','\x57','\x46','\x8a','\x90','\xa7','\x28','\x46','\x5c','\xf8','\x45','\x17','\x23','\x85','\x56','\x44','\xd4','\x1a','\x51','\x68','\x45','\x61','\x44','\x70','\x64','\x2a','\xea','\xed','\x58','\x44','\x42','\x17','\xad','\xd4','\xa0','\x55','\xe8','\x90','\x4d','\x17','\x63','\x41','\xb0','\x77','\x66','\x48','\xd2','\x24','\xf3','\x6e','\xcf','\xb9','\xf7','\x9c','\x73','\x53','\x7b','\x7e','\xab','\xac','\xf2','\xff','\x71','\xef','\x79','\xf7','\xeb','\xdd','\x37','\x49','\xa2','\x28','\x8a','\xa2','\x28','\x8a','\xa2','\x28','\x8a','\xa2','\x28','\xca','\xa5','\xcb','\xeb','\x9b','\x27','\xd7','\x55','\xab','\xbb','\xaa','\x43','\xed','\xf7','\xef','\xc9','\xa5','\xc3','\xd3','\xfb','\xce','\xfd','\x56','\x36','\xb','\x94','\xe7','\x66','\xce','\x7e','\x25','\x18','\x5f','\xfa','\xec','\x99','\xc5','\xf0','\xd1','\xf9','\x3f','\xee','\x38','\x9b','\xc9','\xc4','\xa7','\x9b','\x6e','\x37','\xfd','\x79','\xf8','\x5d','\x9','\x85','\x7','\x76','\x16','\xc4','\x77','\xa8','\xdf','\x98','\x33','\xc7','\xa7','\xcf','\x8d','\x3a','\xf2','\x2d','\xa7','\x3f','\x62','\xcd','\x5f','\xb3','\xd7','\x1d','\x6f','\x69','\x3c','\x95','\xf3','\xe5','\xbf','\x3a','\x7c','\xd1','\x7c','\xcb','\xef','\x9f','\x73','\xe5','\xbf','\xd8','\x82','\xe4','\x1b','\xf3','\xc4','\xdd','\x3c','\xf9','\xf','\x5e','\xa4','\xfb','\x17','\x69','\x3e','\xcf','\x91','\xff','\x1a','\x38','\xdf','\x1a','\xfc','\x43','\x9f','\xff','\x6','\x22','\xdf','\x96','\x22','\x79','\x1b','\xc','\xb4','\x30','\xf9','\xb6','\xd','\x9e','\xa5','\xcd','\x1f','\x3c','\x81','\xcb','\xb7','\x63','\x12','\x69','\x25','\xa6','\x37','\x61','\xf3','\x8d','\x39','\x92','\x11','\xa','\x6c','\xc0','\xe7','\x1b','\xf3','\x18','\x5d','\xfe','\x15','\x3e','\xf9','\xc6','\xfc','\x4a','\x95','\x5f','\x1a','\xf6','\x13','\x68','\x50','\x15','\xe2','\x7','\x7e','\xf9','\xb6','\xc','\x72','\x92','\xfc','\x6b','\x7c','\xf3','\x8d','\x39','\x43','\x91','\x9f','\xa2','\x9f','\xc0','\xb','\xa0','\xe8','\x84','\x6f','\x2','\xf2','\xcd','\xb7','\xe1','\xf9','\x3','\xa8','\x21','\x78','\x5','\xc7','\x83','\x5','\x5e','\x8','\xca','\x37','\xcd','\x2c','\x30','\xdf','\x73','\x8','\x58','\x24','\xb4','\xe','\x3d','\xc6','\xe0','\xa5','\x34','\xc2','\xd6','\x47','\x1','\x8f','\xe0','\x3c','\x3f','\x5','\x9','\xec','\xb','\x17','\x30','\x21','\x4d','\x40','\xd0','\x0','\xc6','\x3c','\x1e','\x20','\x10','\x5c','\x1','\x1d','\x2','\xaa','\xe0','\x7a','\x8a','\x7c','\x63','\xfe','\xf0','\x16','\x98','\xa6','\x11','\xa8','\xfb','\xe6','\xaf','\xa1','\xc9','\x37','\xe6','\x98','\xa7','\xc0','\xfd','\x54','\x2','\x9e','\x33','\x42','\x3a','\x4c','\x25','\xe0','\x39','\x29','\x5e','\x4d','\x96','\xef','\xf9','\x24','\xfe','\x4b','\x27','\x50','\xcf','\x3d','\xf2','\xc9','\x4a','\xb0','\xc3','\x9b','\x1e','\x2','\x64','\x25','\xd8','\xe1','\x47','\xf','\x81','\x71','\x4a','\x81','\x66','\x8e','\xce','\x5f','\x4b','\x99','\xef','\xd3','\x7','\x5f','\xd3','\xa','\xe0','\xfb','\x60','\x98','\x56','\xa0','\x89','\xcd','\x27','\x9a','\x87','\x16','\xb9','\x1','\x29','\x40','\xfa','\xc','\x74','\x78','\x14','\x29','\x30','\x4e','\x2d','\xf0','\x3','\x2e','\x7f','\x80','\x3a','\x1f','\xbb','\x32','\x7b','\x89','\x5e','\xe0','\x49','\x94','\x0','\xe1','\x3c','\x30','\xf','\x6a','\x75','\x9c','\x86','\xed','\xc7','\xfa','\x72','\x4','\x23','\x10','\xbc','\x1f','\xea','\x47','\x86','\x10','\x20','\x7f','\x8','\x3b','\x60','\x46','\x63','\x8a','\xfd','\xc8','\xa','\x10','\x8b','\xe3','\x94','\x23','\xdf','\x7c','\xf','\x17','\x60','\x29','\x1','\xcc','\xea','\xfc','\x21','\x16','\x1','\xc4','\x50','\x14','\x78','\x28','\x51','\x4','\xbc','\xa','\x5b','\x3c','\x2','\xa7','\xa0','\xf9','\xa4','\xcb','\xd1','\xb','\xf8','\xe','\x2a','\x40','\xb8','\x21','\x58','\x2','\x78','\x83','\xf4','\x5','\x93','\x0','\x78','\x77','\xc0','\x30','\x13','\xf5','\xc8','\x80','\x2','\x21','\x47','\xa3','\x4e','\xe','\xc0','\xf2','\x4b','\x5c','\xf9','\xd0','\xe7','\x90','\x7c','\x3d','\xba','\xc0','\xad','\x30','\x81','\x2b','\xd9','\x4','\x80','\xb','\x53','\x96','\xb9','\xb8','\xb','\x70','\x20','\x60','\x1a','\x88','\x2d','\x3f','\xc3','\x4','\x58','\x16','\x3','\x5d','\x60','\x4b','\x73','\xc2','\x93','\x99','\xe5','\xc0','\x96','\x85','\x7c','\x4f','\xa1','\x39','\x9a','\x43','\x4','\xb8','\xa6','\x22','\x3','\x3d','\x25','\xe0','\x59','\xe','\xf5','\x4','\x32','\x88','\x0','\xd7','\x5c','\x68','\xa0','\xa7','\xc6','\xd7','\xc5','\x16','\x78','\x85','\x4f','\x0','\xb6','\x2a','\xdc','\x11','\x5b','\x60','\x2b','\x5f','\x3e','\xac','\xb','\xa6','\x63','\xb','\xb0','\xad','\x87','\xac','\x40','\x16','\x59','\x0','\x36','\x10','\x91','\xbc','\x27','\xea','\xf','\x6c','\x28','\x66','\x14','\x80','\x4d','\x46','\x8c','\x5d','\x0','\x9b','\x8e','\x19','\x5','\x60','\xe7','\xb5','\x7c','\xb','\x22','\xe0','\x9a','\x90','\x71','\x20','\x3a','\x5','\x12','\xd8','\xc1','\x27','\x0','\x3b','\x2f','\xbe','\x8a','\x4f','\x0','\x76','\x42','\xc1','\xb7','\x2d','\x80','\xd','\x3','\x8c','\x1b','\x23','\xe0','\x29','\x15','\xdf','\x9a','\xf0','\x6f','\x98','\x0','\xdf','\xb2','\x1c','\xfa','\xfe','\x94','\x6b','\x2c','\x6','\x96','\x0','\xdf','\xde','\x10','\xfc','\xfe','\x96','\xe4','\xde','\x46','\x1f','\xc0','\x6f','\xb0','\xd3','\x16','\x4b','\x3e','\xe2','\x1d','\xfe','\x34','\x8b','\x0','\xe2','\xb5','\x19','\xcb','\xde','\x8','\x71','\x54','\xcd','\xb2','\x41','\xaf','\xa3','\xde','\x59','\xd1','\xf','\x86','\x75','\xe4','\x1d','\xa','\xaf','\x5b','\xb4','\xe','\x4e','\x6f','\xc4','\xe5','\x27','\xc9','\xa7','\x5e','\x39','\xa3','\xe5','\x72','\xa5','\x32','\x37','\x37','\x3b','\x7b','\xdb','\x2f','\x27','\x67','\x66','\xc6','\x2c','\xbb','\x2c','\xd5','\xb7','\x3e','\xc1','\xc6','\x5b','\xbe','\x7c','\x7b','\xb4','\x4b','\xb9','\x43','\xa5','\x32','\x3b','\xbb','\xfe','\xe4','\xd8','\x58','\xb5','\x3a','\x74','\xcb','\x7f','\x13','\x7b','\xf6','\x1c','\x3c','\xb8','\x79','\xa4','\xc7','\xa1','\x91','\x2d','\x23','\x87','\x6a','\x96','\x2d','\xb5','\xda','\xf6','\xc3','\x87','\x6b','\xdb','\x2c','\x53','\x53','\x53','\x79','\xee','\x11','\xb9','\x9c','\xb4','\x47','\xde','\xfd','\x67','\x14','\xff','\x50','\x51','\xfe','\x37','\x7c','\x3c','\x31','\xcf','\xa4','\x65','\xa8','\xdd','\x6e','\x4f','\x76','\x99','\x68','\xb7','\xcf','\x65','\x2','\xf9','\xce','\x15','\x1b','\xf1','\x77','\x1d','\xfd','\x5','\x5c','\xef','\x53','\x98','\x3e','\x30','\x5a','\x8a','\x4b','\x80','\xed','\x1b','\xab','\x55','\x25','\xe0','\xaa','\x81','\xe8','\x2d','\x90','\x49','\x8','\x44','\x6f','\x1','\x97','\x40','\xa6','\x2','\x97','\x85','\x40','\x4b','\x5','\x54','\xa0','\x38','\x1f','\x76','\x1c','\x1f','\x8a','\xeb','\xa2','\x5d','\x1e','\x5b','\x20','\x8b','\x2d','\x90','\x4b','\x8','\xb8','\x8a','\x50','\x44','\xc0','\x91','\x7f','\x99','\xd4','\x80','\x23','\xbf','\x91','\x4b','\x8','\x44','\x2f','\xc2','\xe8','\x35','\x10','\x5d','\x60','\x55','\x17','\xa1','\x84','\x80','\xf3','\xd2','\x75','\x1e','\x5b','\x20','\x93','\x10','\x88','\x5d','\x84','\xae','\x16','\x10','\x19','\x88','\x56','\x75','\xb','\xc8','\x8','\x38','\xf2','\x45','\xba','\xc0','\x79','\xd5','\x4c','\x42','\x20','\x7e','\xb','\xb8','\x6a','\x40','\x20','\xdf','\xdd','\x2','\x12','\x2','\xce','\x1a','\x88','\x2d','\xa0','\x2d','\x10','\x5d','\x40','\xa4','\xb','\x6','\x63','\xb7','\xc0','\xa0','\x6b','\x36','\x14','\x11','\x58','\xcd','\x5d','\xa0','\x2','\xe8','\x4f','\x6b','\xa9','\x5','\x44','\x5a','\xc0','\x75','\xcb','\xc7','\xe3','\x3','\x6f','\x3c','\xd1','\xbb','\xc0','\xd5','\x2','\xd1','\xbb','\x40','\x9f','\x2','\xef','\x1f','\x1c','\x41','\xe1','\x98','\xb','\x8e','\x8a','\x8','\xb4','\x8a','\x5','\x90','\xdf','\x97','\x7b','\x32','\x5e','\x2c','\xe0','\xf3','\x4b','\x13','\x78','\xa6','\x8b','\x5','\xc0','\x5f','\x34','\x6','\xe1','\xb8','\x6c','\x7','\xbc','\xb1','\x18','\xc8','\xb5','\xc5','\x2','\x3e','\xbf','\x36','\x82','\xc7','\xf1','\xd3','\x4','\x7','\x44','\x4','\x8a','\x1f','\x3','\x91','\x71','\x28','\x71','\x54','\x21','\xf0','\x5b','\xb6','\x60','\xa','\x3f','\x44','\x92','\xa9','\x41','\xc7','\xfe','\x58','\xe2','\x2','\x47','\x97','\x82','\xeb','\xef','\x32','\x3','\x71','\x87','\x82','\x9f','\xa9','\x91','\xea','\x81','\xa4','\xe8','\x1b','\x8','\x91','\xcb','\x3','\x3d','\xd6','\xf6','\xab','\x82','\x47','\xe4','\xf2','\x93','\xe4','\xde','\x95','\xf9','\x4d','\x91','\x2b','\x3c','\xb','\xac','\xbc','\xf6','\xfa','\xb4','\x68','\x7e','\x52','\x5a','\xfe','\xd3','\xd9','\x7f','\xc9','\xe6','\xdb','\x95','\xd9','\xcd','\x4b','\xf2','\xef','\xca','\xa5','\x5','\x92','\x74','\xd3','\x9f','\xb','\xf1','\x8d','\x77','\xe4','\xf3','\x2d','\xa5','\xf7','\x76','\xdf','\x59','\x36','\xe5','\xca','\xfa','\xfd','\x82','\xf','\xe0','\x32','\x5e','\xde','\xfe','\x61','\x6d','\x5b','\x1e','\x2d','\x5e','\x51','\x14','\x45','\x51','\x14','\x45','\x51','\x14','\x45','\x51','\x14','\x45','\xa1','\xe0','\x3c','\xba','\xea','\xc2','\x59','\x1d','\xed','\xc6','\x7b','\x0','\x0','\x0','\x0','\x49','\x45','\x4e','\x44','\xae','\x42','\x60','\x82',};
