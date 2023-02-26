import React from 'react'
import {NavBar, Space, Toast} from 'antd-mobile'
import {useLocation, useNavigate} from 'react-router-dom'

export default () => {
    const navigate = useNavigate()
    const loaction = useLocation()
    const back = () => navigate('./homepage')
    let name = location.href.split('/')[4]
    return (
        <>
            <NavBar onBack={back} style={{
                background: "rgba(150, 193, 116, 0.14)",
                borderRadius: "16px",
                boxShadow: "0 4px 30px rgba(0, 0, 0, 0.1)",
                backdropFilter:"blur(3.2px)"
            }}>
                {name}
            </NavBar>

        </>
    )
}
