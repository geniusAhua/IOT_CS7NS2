import React, {useState} from 'react'
import { Button,Picker,ConfigProvider } from 'antd-mobile'
import moment from 'moment';
import 'moment/locale/zh-cn';
import zhCN from "antd-mobile/es/locales/zh-CN";
import { DemoBlock } from '../components/demos-util'
import { Avatar, Space } from 'antd-mobile';
import './avatar.less'

moment.locale('en');

export default function BasicDemo() {
    const [visible, setVisible] = useState(false)
    const [value, setValue] = useState<(string | null)[]>(['0'])
    const demoAvatarImages = [
        'https://images.unsplash.com/photo-1548532928-b34e3be62fc6?ixlib=rb-1.2.1&q=80&fm=jpg&crop=faces&fit=crop&h=200&w=200&ixid=eyJhcHBfaWQiOjE3Nzg0fQ',
        'https://images.unsplash.com/photo-1493666438817-866a91353ca9?ixlib=rb-0.3.5&q=80&fm=jpg&crop=faces&fit=crop&h=200&w=200&s=b616b2c5b373a80ffc9636ba24f7a4a9',
        'https://images.unsplash.com/photo-1542624937-8d1e9f53c1b9?ixlib=rb-1.2.1&q=80&fm=jpg&crop=faces&fit=crop&h=200&w=200&ixid=eyJhcHBfaWQiOjE3Nzg0fQ',
        'https://images.unsplash.com/photo-1546967191-fdfb13ed6b1e?ixlib=rb-1.2.1&q=80&fm=jpg&crop=faces&fit=crop&h=200&w=200&ixid=eyJhcHBfaWQiOjE3Nzg0fQ',
    ]

    const basicColumns = [
        [
            { label: <Avatar className="avatar" src={demoAvatarImages[0]}  />, value: '0' },
            { label: <Avatar className="avatar" src={demoAvatarImages[1]}  />, value: '1' },
            { label: <Avatar className="avatar" src={demoAvatarImages[2]}  />, value: '2' },
            { label: <Avatar className="avatar" src={demoAvatarImages[3]}  />, value: '3' },
        ]
    ]
    console.log(Number(value==null?null:value))
    return (
        <>
            <DemoBlock title='' background=''>
                <Space block direction='vertical'>
                    <Avatar src={demoAvatarImages[Number(value==null?null:value)]} style={{width:"50%",height:"15vh",marginLeft:"22vw"}}/>
                </Space>
            </DemoBlock>

            <Button
                onClick={() => {
                    setVisible(true)
                }}
                style={{marginLeft:"34vw",marginTop:"1vh",borderRadius:"50px"}}
            >
                choose
            </Button>
            <ConfigProvider locale={zhCN}>
            <Picker
                columns={basicColumns}
                visible={visible}
                onClose={() => {
                    setVisible(false)
                }}
                value={value}
                onConfirm={v => {
                    setValue(v)
                }}
            />
            </ConfigProvider>
        </>
    )
}
