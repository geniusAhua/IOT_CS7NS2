import React, { useState, useRef, useEffect } from 'react'
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
import Footer from "../components/footer";
import { GoogleMap, LoadScript, useJsApiLoader,MarkerF } from '@react-google-maps/api';

const containerStyle = {
    width: screen.width,
    height: screen.height
};

const center = {
    lat: 53.49332,
    lng: -6.31718
};
const mapId:string = 'd07532df77f9d9a5'
const options = {
    // 将 `language` 属性添加到 `options` 对象中
    ...{
        zoomControl: true,
        streetViewControl: false,
    },
    language: "en-GB" // 设置地图的语言为中文
};


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
    const { isLoaded } = useJsApiLoader({
        id: 'd07532df77f9d9a5',
        googleMapsApiKey: 'AIzaSyBxhljI-42-8Sn2UOAVf3Cw_9lH4otQ6vY',
        libraries: ['geometry', 'drawing'],
    });

    const [markers,setMarkers] =useState({
        marker1: {
            lat: 53.49332,
            lng: -6.31718
        },
        marker2: {
            lat: 54.49332,
            lng: -6.31718
        }
    })

    // BinDataApi({params}).then(res=>{
    //     let positions = JSON.parse(JSON.stringify(res))
    //     const markers = [...positions]
    //     markers.push(position)
    //     setMarkers({ markers })
    // }).catch(function(err){
    //     console.log(err)
    // })

    return (
        <div>
            {isLoaded &&<GoogleMap
                    mapContainerStyle={containerStyle}
                    center={center}
                    zoom={10}
                    options={options}
               >
                {  Object.values(markers).map((index,position)=>
                    <MarkerF
                        position={index} key={position}
                    />
                )
                }
            </GoogleMap>
            }
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
        </div>
    )
}
