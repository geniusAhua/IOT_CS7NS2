import React, { useState, useRef } from 'react'
import {
    FloatingPanel,
    SearchBar,
    Avatar,
    Space,
    Card,
    List,
} from 'antd-mobile'
import {
    SoundOutline,
} from 'antd-mobile-icons'
import Open_bin from "../bin/open_bin.jpg";
// @ts-ignore
import styles from '../less/google.less'

const locations = [
    {
        name: 'binA',
        avatar: Open_bin,
    },
    {
        name: 'binB',
        avatar: Open_bin,
    },
    {
        name: 'binC',
        avatar: Open_bin,
    },
    {
        name: 'binD',
        avatar: Open_bin,
    },
]

const nearbyList = [
    {
        icon: <SoundOutline />,
        name: 'route',
    },
]

const anchors = [72, 72 + 119, window.innerHeight * 0.8]

export default () => {
    const [focus, setFocus] = useState(false)

    return (
        <>
            <iframe src='https://amap.com' className={styles.map} style={{width:"200vw",height:"300vh"}}/>

            <FloatingPanel anchors={anchors}>
                <Space block className={styles.search}>
                    <SearchBar
                        placeholder='Search for a bin'
                        showCancelButton
                        onFocus={() => {
                            setFocus(true)
                        }}
                        onBlur={() => {
                            setFocus(false)
                        }}
                    />
                    {!focus && <Avatar src='' className={styles.avatar} />}
                </Space>
                <List header='Bin' style={{ '--border-bottom': 'none' }}>
                    <Card>
                        <Space block justify='around'>
                            {locations.map(item => (
                                <div key={item.name} className={styles.item}>
                                    <Avatar src={item.avatar} style={{ margin: 'auto' }} />
                                    {item.name}
                                </div>
                            ))}
                        </Space>
                    </Card>
                </List>

                <List header='Find Nearby Bin'>
                    {nearbyList.map(item => (
                        <List.Item prefix={item.icon} key={item.name}>
                            {item.name}
                        </List.Item>
                    ))}
                </List>
            </FloatingPanel>
        </>
    )
}
