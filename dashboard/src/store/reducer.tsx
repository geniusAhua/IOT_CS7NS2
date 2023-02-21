import { createSlice, PayloadAction } from '@reduxjs/toolkit'
import type { RootState } from '../store/index'

interface CounterState { // 定义初始化状态的类型
    value: number
}
const initialState: CounterState = { // 初始化状态
    value: 0,
}
export const counterSlice = createSlice({
    name: 'counter',
    initialState,
    reducers: {
        increment: (state) => {
            state.value += 1
        },
        decrement: (state) => {
            state.value -= 1
        },
        incrementByAmount: (state, action: PayloadAction<number>) => {
            state.value = action.payload
        },
    },
})

export const { increment, decrement, incrementByAmount } = counterSlice.actions // 导出操作state的喊出
export const selectCount = (state: RootState) => state
export default counterSlice.reducer // 导出当前reducer在store/index.ts中记性全局挂载（这种也可以不用挂载到全局）
