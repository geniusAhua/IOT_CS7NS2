import React, { useState, useRef, useEffect, useCallback, useMemo } from 'react'
import {
    FloatingPanel,
    SearchBar,
    Avatar,
    Space,
    Card,
    List,
    Button,
} from 'antd-mobile'
import {
    SoundOutline,
} from 'antd-mobile-icons'
import Open_bin from "../bin/open_bin.jpg";
import Map from './Load';
// @ts-ignore
import styles from '../less/google.less'
import Footer from "../components/footer";
import { GoogleMap, LoadScript, useJsApiLoader,MarkerF, Marker, InfoWindow, DirectionsService, DirectionsRenderer, TravelMode, useLoadScript } from '@react-google-maps/api';
import { BinDataApi } from '../request/api';
import PlaceSearch from './PlaceSearch';
import {useLocation, useNavigate, useSearchParams } from "react-router-dom";



const containerStyle = {
    width: screen.width,
    height: screen.height
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
    const [response, setResponse] = useState(null);
    const navigate = useNavigate()
    let next: null = null
    // 处理路线响应
    const handleDirectionsResponse = useCallback((response:any) => {
        let pre = response
      if(pre != next){
          setResponse(response)
          next = response
      }
    }, []);

    const [markers,setMarkers] =useState({
        marker1: {
            lat: 53.4,
            lng: -6.2
        },
        marker2: {
            lat: 53.29,
            lng: -6.3
        }
    })
    const center = {
        lat: 53.49332,
        lng: -6.3
    };

    let params={id:1}
    const api =()=> {
        BinDataApi({params}).then(res => {
           let positions = {
                lat: parseFloat(JSON.parse(JSON.stringify(res)).longitude),
                lng: parseFloat(JSON.parse(JSON.stringify(res)).latitude)
            }
            markers['marker1'] = positions
            setMarkers(markers)
            console.log(markers)
        }).catch(function (err) {
            console.log(err)
        })
    }

    const [showComponent, setShowComponent] = useState(false);
    useEffect(() => {
        const timeoutId = setTimeout(() => {
            setShowComponent(true);
        }, 3000);
        return () => {
            clearTimeout(timeoutId);
        };
    }, []);

    const [selectedMarker, setSelectedMarker] = useState(null);
    const [geocodeOrigin, setGeocodeOrigin] = useState({});
    const [geocodeDestination, setGeocodeDestination] = useState([])
    const [title, setTitle] = useState(0);
    const onSelectMarker = useCallback((marker: { lat: number; lng: number},position:number) => {
        // @ts-ignore
        setSelectedMarker(marker)
        setTitle(position)
    },[]);
    const bins = {
        binA:["Dorset Street Lower","Trinity College Library, College Green, South-East Inner City, Dublin 2"],
        binB:["O'Neills Victorian Pub & Townhouse","Trinity College Library, College Green, South-East Inner City, Dublin 2"],
        binC:["Phoenix Park, Dublin 8","Trinity College Library, College Green, South-East Inner City, Dublin 2"],
        binD:["Dockers","Trinity College Library, College Green, South-East Inner City, Dublin 2"],
    }
    let origin = 'Origin'
    let destination = 'Destination'
    if(localStorage.getItem(origin)!='Origin') {
        origin = localStorage.getItem("origin")
        destination = localStorage.getItem("destination")
        console.log(origin)
    }

    const libraries = ['places']
    const { isLoaded, loadError } = useLoadScript({
        googleMapsApiKey: 'AIzaSyBxhljI-42-8Sn2UOAVf3Cw_9lH4otQ6vY',
        libraries,
    });

    if(loadError) return 'Error loading maps';
    if(!isLoaded) return 'Loading Maps';

    const onSubmint=(props:any)=>{
        localStorage.setItem("origin",bins[props][0])
        localStorage.setItem("destination",bins[props][1])
        location.reload()
    }

    return (
        <div>
            <div style={{width:"100%",height:"100%"}}>
                <Map
                    origin={origin}
                    destination={destination}
                />
            </div>
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
                    {!focus && <Avatar src={Open_bin} className={styles.avatar}/>}
                </Space>
                <List header='Bin' style={{ '--border-bottom': 'none' }}>
                    <Card>
                        <Space block justify='around'>
                            {locations.map(item => (
                                <div key={item.name} className={styles.item}>
                                    <Avatar src={item.avatar} style={{ margin: 'auto' , cursor:'pointer'}} onClick={()=>onSubmint(item.name)}/>
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
